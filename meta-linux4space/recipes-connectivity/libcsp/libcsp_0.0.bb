LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "git://github.com/libcsp/libcsp.git;branch=libcsp-2-0;protocol=https"

SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

inherit cmake pkgconfig

FILES_${PN} += "/usr/lib/lib*.so.*"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
INSANE_SKIP_${PN} += "dev-so"
