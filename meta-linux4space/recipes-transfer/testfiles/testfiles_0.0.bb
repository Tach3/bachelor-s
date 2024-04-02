SUMMARY = "A simple recipe to copy files to rootfs and compile them so they're prepared"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://csp_client.c \
	   file://csp_client_posix.c \
	   file://csp_server.c \
	   file://csp_server_client.c \
	   "
DEPENDS = "libcsp"
RDEPENDS:${PN} += "libcsp"
S = "${WORKDIR}"

MY_FILES = "${THISDIR}/files/*"
MY_DESTINATION = "/home/root/testdata"

INSANE_SKIP:${PN} += "ldflags"

do_compile() {
	${CC} `pkg-config --cflags --libs libcsp` ${CFLAGS} csp_server.c -o server -lcsp
	${CC} `pkg-config --cflags --libs libcsp` ${CFLAGS} csp_client.c csp_client_posix.c -o client -lcsp
	${CC} `pkg-config --cflags --libs libcsp` ${CFLAGS} csp_server_client.c -o both -lcsp
}
do_install() {
	install -d ${D}${bindir}
	install -m 0755 server ${D}${bindir}
	install -m 0755 client ${D}${bindir}
	install -m 0755 both ${D}${bindir}

	install -d ${D}${MY_DESTINATION}
        cp -r ${MY_FILES} ${D}${MY_DESTINATION}
}

FILES:${PN} += "${MY_DESTINATION}"