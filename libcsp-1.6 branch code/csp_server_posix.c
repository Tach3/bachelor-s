#include <linux/reboot.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/reboot.h>

int csp_sys_reboot(void) {
	sync();
	reboot(LINUX_REBOOT_CMD_RESTART);
}
