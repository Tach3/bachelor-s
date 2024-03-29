SUMMARY = "recipe to install libcsp"
HOMEPAGE = "https://www.libcsp.org"
SECTION = "libs"
LICENSE = "LGPL-2.1-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/LGPL-2.1-only;md5=1a6d268fd218675ffea8be556788b780"
DEPENDS = "python3 zeromq libsocketcan"
PV = "1.6"
SRC_URI = "git://github.com/libcsp/libcsp.git;branch=libcsp-1;protocol=https"
SRCREV = "87006959696c78f70535ab382b0bcd4cb5a6558d"
S = "${WORKDIR}/git"

inherit pkgconfig

do_configure() {
    # Modify waf script
    sed -i '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' ${S}/waf
    
    # Modify wscript file
    sed -i '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' ${S}/wscript
    
    # Execute the modified scripts
    cd ${S}
    ./waf configure --toolchain=arm-linux --with-os=posix --with-driver-usart=linux --enable-can-socketcan --enable-if-zmqhub --enable-shlib --prefix=/usr --install-csp --enable-python3-bindings
}

do_install() {
    cd ${S}
    ./waf build install --destdir=${D} --install-csp
    rm -f ${D}${libdir}/libcsp.a
}

PACKAGES = "${PN} ${PN}-dev ${PN}-dbg"

FILES:${PN} += "${libdir}/*"
FILES_${PN}-dbg += "${libdir}/.debug/"
FILES_${PN}-dev += "${includedir}"
