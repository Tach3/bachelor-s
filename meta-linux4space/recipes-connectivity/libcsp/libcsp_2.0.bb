SUMMARY = "CubeSat Space Protocol library"
DESCRIPTION = "Small protocol stack designed to be used \
               with clusters of embedded devices. \
               "
HOMEPAGE = "https://www.libcsp.org"
SECTION = "libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=2915dc85ab8fd26629e560d023ef175c"
PV = "2.0"
SRC_URI = "git://github.com/libcsp/libcsp.git;branch=libcsp-2-0;protocol=https"
SRCREV = "ac58ff917e650ef70eb405a7f598fcc9e4c45cf9"
S = "${WORKDIR}/git"

PACKAGECONFIG_CONFARGS +="--toolchain=${TARGET_SYS} \
			  --with-os=posix \
			  --prefix=/usr \
			  --enable-shlib \
			  --enable-crc32 \
			  --install-csp \
			  "

EXTRA_OEWAF_INSTALL += "--destdir=${D} \
			--install-csp  \
			"

inherit waf

PACKAGES = "${PN} ${PN}-dev ${PN}-dbg ${PN}-staticdev"

FILES:${PN} += "${libdir}/*.so ${prefix}/csp"
FILES:${PN}-dbg += "${libdir}/.debug/"
FILES:${PN}-dev += "${includedir}"
FILES:${PN}-staticdev += "${libdir}/*.a"
