/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/usart.h>
#include <csp/drivers/can_socketcan.h>
#include <csp/interfaces/csp_if_zmqhub.h>

/* Server port, the port the server listens on for incoming connections from the client. */
#define MY_SERVER_PORT		10

/* Commandline options */
static uint8_t server_address = 255;

/* test mode, used for verifying that host & client can exchange packets over the loopback interface */
static bool test_mode = false;
static unsigned int server_received = 0;

/* Server task - handles requests from clients */
CSP_DEFINE_TASK(task_server) {

	csp_log_info("Server task started");

	/* Create socket with no specific socket options, e.g. accepts CRC32, HMAC, XTEA, etc. if enabled during compilation */
	csp_socket_t *sock = csp_socket(CSP_SO_NONE);

	/* Bind socket to all ports, e.g. all incoming connections will be handled here */
	csp_bind(sock, CSP_ANY);

	/* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
	csp_listen(sock, 10);

	/* Wait for connections and then process packets on the connection */
	while (1) {

		/* Wait for a new connection, 10000 mS timeout */
		csp_conn_t *conn;
		if ((conn = csp_accept(sock, 10000)) == NULL) {
			/* timeout */
			continue;
		}

		/* Read packets on connection, timout is 100 mS */
		csp_packet_t *packet;
		while ((packet = csp_read(conn, 50)) != NULL) {
			switch (csp_conn_dport(conn)) {
			case MY_SERVER_PORT:
				/* Process packet here */
				csp_log_info("Packet received on MY_SERVER_PORT: %s", (char *) packet->data);
				csp_buffer_free(packet);
				++server_received;
				break;

			default:
				/* Call the default CSP service handler, handle pings, buffer use, etc. */
				csp_service_handler(conn, packet);
				break;
			}
		}

		/* Close current connection */
		csp_close(conn);

	}

	return CSP_TASK_RETURN;

}
/* End of server task */

/* main - initialization of CSP and start of server/client tasks */
int main(int argc, char * argv[]) {

    uint8_t address = 1;
    csp_debug_level_t debug_level = CSP_INFO;
#if (CSP_HAVE_LIBSOCKETCAN)
    const char * can_device = NULL;
#endif
    const char * kiss_device = NULL;
#if (CSP_HAVE_LIBZMQ)
    const char * zmq_device = NULL;
#endif
    const char * rtable = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "a:d:r:c:k:z:tR:h")) != -1) {
        switch (opt) {
            case 'a':
                address = atoi(optarg);
                break;
            case 'd':
                debug_level = atoi(optarg);
                break;
            case 'r':
                server_address = atoi(optarg);
                break;
#if (CSP_HAVE_LIBSOCKETCAN)
            case 'c':
                can_device = optarg;
                break;
#endif
            case 'k':
                kiss_device = optarg;
                break;
#if (CSP_HAVE_LIBZMQ)
            case 'z':
                zmq_device = optarg;
                break;
#endif
            case 't':
                test_mode = true;
                break;
            case 'R':
                rtable = optarg;
                break;
            default:
                printf("Usage:\n"
                       " -a <address>     local CSP address\n"
                       " -d <debug-level> debug level, 0 - 6\n"
                       " -r <address>     run client against server address\n"
                       " -c <can-device>  add CAN device\n"
                       " -k <kiss-device> add KISS device (serial)\n"
                       " -z <zmq-device>  add ZMQ device, e.g. \"localhost\"\n"
                       " -R <rtable>      set routing table\n"
                       " -t               enable test mode\n");
                exit(1);
                break;
        }
    }

    /* enable/disable debug levels */
    for (csp_debug_level_t i = 0; i <= CSP_LOCK; ++i) {
        csp_debug_set_level(i, (i <= debug_level) ? true : false);
    }

    csp_log_info("Initialising CSP");

    /* Init CSP with address and default settings */
    csp_conf_t csp_conf;
    csp_conf_get_defaults(&csp_conf);
    csp_conf.address = address;
    int error = csp_init(&csp_conf);
    if (error != CSP_ERR_NONE) {
        csp_log_error("csp_init() failed, error: %d", error);
        exit(1);
    }

    /* Start router task with 10000 bytes of stack (priority is only supported on FreeRTOS) */
    csp_route_start_task(500, 0);

    /* Add interface(s) */
    csp_iface_t * default_iface = NULL;
    if (kiss_device) {
        csp_usart_conf_t conf = {
            .device = kiss_device,
            .baudrate = 115200, /* supported on all platforms */
            .databits = 8,
            .stopbits = 1,
            .paritysetting = 0,
            .checkparity = 0};
        error = csp_usart_open_and_add_kiss_interface(&conf, CSP_IF_KISS_DEFAULT_NAME,  &default_iface);
        if (error != CSP_ERR_NONE) {
            csp_log_error("failed to add KISS interface [%s], error: %d", kiss_device, error);
            exit(1);
        }
    }
#if (CSP_HAVE_LIBSOCKETCAN)
    if (can_device) {
        error = csp_can_socketcan_open_and_add_interface(can_device, CSP_IF_CAN_DEFAULT_NAME, 0, false, &default_iface);
        if (error != CSP_ERR_NONE) {
            csp_log_error("failed to add CAN interface [%s], error: %d", can_device, error);
            exit(1);
        }
    }
#endif
#if (CSP_HAVE_LIBZMQ)
    if (zmq_device) {
        error = csp_zmqhub_init(csp_get_address(), zmq_device, 0, &default_iface);
        if (error != CSP_ERR_NONE) {
            csp_log_error("failed to add ZMQ interface [%s], error: %d", zmq_device, error);
            exit(1);
        }
    }
#endif

    if (rtable) {
        error = csp_rtable_load(rtable);
        if (error < 1) {
            csp_log_error("csp_rtable_load(%s) failed, error: %d", rtable, error);
            exit(1);
        }
    } else if (default_iface) {
        csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);
    } else {
        /* no interfaces configured - run server and client in process, using loopback interface */
        server_address = address;
    }

    printf("Connection table\r\n");
    csp_conn_print_table();

    printf("Interfaces\r\n");
    csp_route_print_interfaces();

    printf("Route table\r\n");
    csp_route_print_table();

    /* Start server thread */
    if ((server_address == 255) ||  /* no server address specified, I must be server */
        (default_iface == NULL)) {  /* no interfaces specified -> run server & client via loopback */
        csp_thread_create(task_server, "SERVER", 1000, NULL, 0, NULL);
    }

    /* Wait for execution to end (ctrl+c) */
    while(1) {
        csp_sleep_ms(3000);

        if (test_mode) {
            /* Test mode is intended for checking that host & client can exchange packets over loopback */
            if (server_received < 5) {
                csp_log_error("Server received %u packets", server_received);
                exit(1);
            }
            csp_log_info("Server received %u packets", server_received);
            exit(0);
        }
    }

    return 0;
}
