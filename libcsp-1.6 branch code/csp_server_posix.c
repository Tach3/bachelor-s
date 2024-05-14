#include <linux/reboot.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/reboot.h>
#include <csp/csp.h>
#include <stdlib.h>

void is_command(csp_packet_t *packet){
	const char* new_message = NULL;
	if ((strncmp(packet->data, "c ", 2) == 0)){
		new_message = packet->data + 2;
		system(new_message);
	}else {
		csp_log_info("Packet received on MY_SERVER_PORT: %s", (char *) packet->data);
	}
}

int csp_sys_reboot(void) {
	sync();
	reboot(LINUX_REBOOT_CMD_RESTART);
}
