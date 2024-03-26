SUMMARY = "A simple recipe to copy files to rootfs and compile them so they're prepared"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

MY_FILES = "${THISDIR}/data/*"
MY_DESTINATION = "/home/root/testdata"


DEPENDS = "libcsp"

do_install(){
        install -d ${D}${MY_DESTINATION}
        cp -r ${MY_FILES} ${D}${MY_DESTINATION}
}

do_compile(){
	install -d ${D}${MY_DESTINATION}
	${CC} ${CFLAGS} -o ${MY_DESTINATION}/server ${MY_DESTINATION}/csp_server.c ${MY_DESTINATION}/csp_server_posix.c -lcsp
	${CC} ${CFLAGS} -o ${MY_DESTINATION}/client ${MY_DESTINATION}/csp_client.c ${MY_DESTINATION}/csp_client_posix.c -lcsp
}

FILES:${PN} += "${MY_DESTINATION}"
