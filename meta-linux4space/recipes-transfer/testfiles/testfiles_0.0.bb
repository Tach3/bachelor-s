SUMMARY = "A simple recipe to copy files to rootfs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

MY_FILES = "${THISDIR}/data/*"
MY_DESTINATION = "/home/root/testdata"

do_install(){
        install -d ${D}${MY_DESTINATION}
        cp -r ${MY_FILES} ${D}${MY_DESTINATION}
}

FILES:${PN} += "${MY_DESTINATION}"
