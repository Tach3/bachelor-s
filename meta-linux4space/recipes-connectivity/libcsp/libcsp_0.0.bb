LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "git://github.com/libcsp/libcsp.git;branch=develop;protocol=https"
SRC_URI[sha256sum] = "cb46fd0478cee9ff8918e13da9b520b366b47100fa706c0cf2775b693034d52b"
SRCREV = "${AUTOREV}"
DEPENDS = "zeromq libsocketcan"
#OECMAKE_FIND_ROOT_PATH_MODE_PROGRAM = "BOTH"
S = "${WORKDIR}/git"

inherit cmake pkgconfig

FILES_${PN} += "/usr/lib/lib*.so.*"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
INSANE_SKIP_${PN} += "dev-so"
