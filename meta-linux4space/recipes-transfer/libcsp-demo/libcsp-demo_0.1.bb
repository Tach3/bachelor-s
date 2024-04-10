SUMMARY = "A simple recipe to copy files to rootfs and compile them so they're prepared"
DESCRIPTION = "recipe that copies .c code demo to /home/linuxforspace/testdata \
			   and compiles them into binary that's installed as a program. \
			   Usage is server || client || server_client \
               "
LICENSE = "LGPL-2.1-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/LGPL-2.1-only;md5=1a6d268fd218675ffea8be556788b780"
DEPENDS = "libcsp"
RDEPENDS:${PN} += "libcsp"

SRC_URI = "git://gitlab.com/linux4space/linux4space-applications/linux4space-libcsp-example.git;branch=main;protocol=https"
SRCREV = "7d22b0edd59318e886bd0928eb74025a0a8d7f15"
S = "${WORKDIR}/git"

MY_DESTINATION = "/home/linuxforspace/testdata"

INSANE_SKIP:${PN} += "ldflags"

do_compile() {
	cd ${S}/code_example
	${CC} `pkg-config --cflags --libs libcsp` ${CFLAGS} csp_server.c csp_server_posix.c -o server -lcsp
	${CC} `pkg-config --cflags --libs libcsp` ${CFLAGS} csp_client.c csp_client_posix.c -o client -lcsp
	${CC} `pkg-config --cflags --libs libcsp` ${CFLAGS} csp_server_client.c -o server_client -lcsp
}
do_install() {
	cd ${S}/code_example
	install -d ${D}${bindir}
	install -m 0755 server ${D}${bindir}
	install -m 0755 client ${D}${bindir}
	install -m 0755 server_client ${D}${bindir}

	install -d ${D}${MY_DESTINATION}
    cp -r ${S}/code_example/* ${D}${MY_DESTINATION}
}

FILES:${PN} += "${MY_DESTINATION}"
