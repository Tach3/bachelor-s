
inherit extrausers

IMAGE_INSTALL:append = " sudo"

# This password is generated with `openssl passwd -6 password`, 
# where -6 stands for SHA-512 hashing alorithgm
# The resulting string is in format $<ALGORITHM_ID>$<SALT>$<PASSWORD_HASH>,
# the dollar signs have been escaped
# This'll allow user to login with the least secure password there is, "password" (without quotes)
PASSWD = "\$6\$vRcGS0O8nEeug1zJ\$YnRLFm/w1y/JtgGOQRTfm57c1.QVSZfbJEHzzLUAFmwcf6N72tDQ7xlsmhEF.3JdVL9iz75DVnmmtxVnNIFvp0"

# This creates a user with name linux4space and UID 1200. 
# The password is stored in the aforementioned PASSWD variable
# and home-folder is /home/linuxforspace, and the login-shell is set as sh.
# Finally, this user is added to the sudo-group.
EXTRA_USERS_PARAMS:append = "\
    useradd -u 1200 -d /home/linuxforspace -s /bin/sh -p '${PASSWD}' linuxforspace; \
    usermod -a -G sudo linuxforspace; \
    "
# Here we give sudo access to sudo members
update_sudoers(){
    sed -i 's/# %sudo/%sudo/' ${IMAGE_ROOTFS}/etc/sudoers
}

ROOTFS_POSTPROCESS_COMMAND += "update_sudoers;"