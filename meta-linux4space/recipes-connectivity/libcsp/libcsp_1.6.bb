SUMMARY = "recipe to install libcsp"
LICENSE = "LGPL-2.1-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/LGPL-2.1-only;md5=1a6d268fd218675ffea8be556788b780"
SECTION = "libs"
SRC_URI = "git://github.com/libcsp/libcsp.git;branch=libcsp-1;protocol=https"
SRCREV = "${AUTOREV}"
DEPENDS = "python3"
S = "${WORKDIR}/git"

inherit waf

do_configure() {
    # Modify waf script
    sed -i '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' ${S}/waf
    
    # Modify wscript file
    sed -i '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' ${S}/wscript
    
    # Execute the modified scripts
    cd ${S}
    ./waf configure --toolchain=arm-linux --with-os=posix --with-driver-usart=linux --enable-shlib --prefix=/usr --install-csp
}

do_install() {
    cd ${S}
    ./waf build install --destdir=${D} --install-csp
    rm -f ${D}${libdir}/libcsp.a
}

FILES:${PN} += "${libdir}/*"
FILES_${PN}-dbg += "${libdir}/.debug/"
FILES_${PN}-dev += "${includedir}"
PACKAGES = "${PN} ${PN}-dev ${PN}-dbg"