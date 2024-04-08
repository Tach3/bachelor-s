SUMMARY = "recipe to install libcsp"
DESCRIPTION = "CubeSat protocol library writte in C and built with waf. \
               This recipe is for latest stable release libcsp 1.6. \
               Definition of drivers is required in .bbappend file \
               "
HOMEPAGE = "https://www.libcsp.org"
SECTION = "libs"
LICENSE = "LGPL-2.1-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/LGPL-2.1-only;md5=1a6d268fd218675ffea8be556788b780"
DEPENDS = "python3"
PV = "1.6"
SRC_URI = "git://github.com/libcsp/libcsp.git;branch=libcsp-1;protocol=https"
SRCREV = "87006959696c78f70535ab382b0bcd4cb5a6558d"
S = "${WORKDIR}/git"

# Custom config variables. Modify in .bbappend.
OS = ""
DRIVERS = ""
FLAGS = ""
INSTALL_LOCATION = ""

inherit pkgconfig

do_configure() {
    # Modify waf script to change python to python3
    sed -i '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' ${S}/waf
    
    # Modify wscript file to change python to python3
    sed -i '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' ${S}/wscript
    
    # Execute the modified scripts
    cd ${S}
    ./waf configure --toolchain=${TARGET_SYS} ${OS} ${DRIVERS} --enable-shlib ${INSTALL_LOCATION} --install-csp ${FLAGS}
}

do_install() {
    cd ${S}
    ./waf build install --destdir=${D} --install-csp
    rm -f ${D}${libdir}/libcsp.a
}

PACKAGES = "${PN} ${PN}-dev ${PN}-dbg"

FILES:${PN} += "${libdir}/*"
FILES:${PN}-dbg += "${libdir}/.debug/"
FILES:${PN}-dev += "${includedir}"
