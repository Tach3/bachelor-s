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
/*
Code has been modified, from csp_server_client.c into this. 10.04.2024
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

char* get_message(char *string);

/* Server port, the port the server listens on for incoming connections from the client. */
#define MY_SERVER_PORT		10

/* Commandline options */
static uint8_t server_address = 255;

/* test mode, used for verifying that host & client can exchange packets over the loopback interface */
static bool test_mode = false;
static unsigned int successful_ping = 0;

/*message mode*/
static bool message_mode = false;
static char * message;

/* Client task sending requests to server task */
CSP_DEFINE_TASK(task_client) {

	csp_log_info("Client task started");

	unsigned int count = 0;

	while (1) {

		csp_sleep_ms(test_mode ? 200 : 1000);

		/* Send ping to server, timeout 1000 mS, ping size 100 bytes */
		int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
		csp_log_info("Ping address: %u, result %d [mS]", server_address, result);

        /* Send reboot request to server, the server has no actual implementation of csp_sys_reboot() and fails to reboot */
        //csp_reboot(server_address);
        //csp_log_info("reboot system request sent to address: %u", server_address);

        if (result > 0) {
            ++successful_ping;
        }

		/* Send data packet (string) to server */

		/* 1. Connect to host on 'server_address', port MY_SERVER_PORT with regular UDP-like protocol and 1000 ms timeout */
		csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, MY_SERVER_PORT, 1000, CSP_O_NONE);
		if (conn == NULL) {
			/* Connect failed */
			csp_log_error("Connection failed");
			return CSP_TASK_RETURN;
		}

		/* 2. Get packet buffer for message/data */
		csp_packet_t * packet = csp_buffer_get(100);
		if (packet == NULL) {
			/* Could not get buffer element */
			csp_log_error("Failed to get CSP buffer");
			return CSP_TASK_RETURN;
		}

		/* 3. Copy data to packet */
        if(message_mode){
            csp_log_info("Message to send: ");
            message = get_message(message);
            snprintf((char *) packet->data, csp_buffer_data_size(), "%s" , message);
            free(message);
        } else {
		    snprintf((char *) packet->data, csp_buffer_data_size(), "Hello World (%u)", ++count);
        }

		/* 4. Set packet length */
		packet->length = (strlen((char *) packet->data) + 1); /* include the 0 termination */

		/* 5. Send packet */
		if (!csp_send(conn, packet, 1000)) {
			/* Send failed */
			csp_log_error("Send failed");
			csp_buffer_free(packet);
		}

		/* 6. Close connection */
		csp_close(conn);
	}

	return CSP_TASK_RETURN;
}
/* End of client task */

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
    while ((opt = getopt(argc, argv, "a:d:r:c:k:z:tR:mh")) != -1) {
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
            case 'm':
                message_mode = true;
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
                       " -m               enable message mode\n"
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

    /* Start client thread */
    if ((server_address != 255) ||  /* server address specified, I must be client */
        (default_iface == NULL)) {  /* no interfaces specified -> run server & client via loopback */
        csp_thread_create(task_client, "CLIENT", 1000, NULL, 0, NULL);
    }

    /* Wait for execution to end (ctrl+c) */
    while(1) {
        csp_sleep_ms(3000);

        if (test_mode) {
            /* Test mode is intended for checking that host & client can exchange packets over loopback */
            if (successful_ping < 5) {
                csp_log_info("Client successfully pinged the server %u times\n", successful_ping);
                exit(1);
            }
            csp_log_info("Client successfully pinged the server %u times\n", successful_ping);
            exit(0);
        }
    }

    return 0;
}
