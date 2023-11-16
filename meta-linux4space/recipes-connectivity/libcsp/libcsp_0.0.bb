LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "git://github.com/libcsp/libcsp.git;branch=develop;protocol=https"

SRCREV = "${AUTOREV}"
DEPENDS = "zeromq libsocketcan"

S = "${WORKDIR}/git"

inherit cmake pkgconfig

FILES_${PN} += "/usr/lib/lib*.so.*"

#library is not versioned
SOLIBS = ".so"
FILES_SOLIBSDEV = ""
INSANE_SKIP_${PN} += "dev-so"
