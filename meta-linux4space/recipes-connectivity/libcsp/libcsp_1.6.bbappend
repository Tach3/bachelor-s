# Decide which drivers need to be installed for your project. Libcsp offers USART, CAN and ZEROMQ

DRIVERS += "--with-driver-usart=linux --enable-can-socketcan --enable-if-zmqhub"


# Flags for libcsp configuration. Check out libcsp 1.6 wscript for more options.

FLAGS += "--enable-python3-bindings --enable-crc32"