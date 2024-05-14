# Depends variable -- if socketcan and zmqhub is in drivers, it has to be in depends too
DEPENDS += " zeromq libsocketcan"

# Decide which drivers need to be installed for your project. Libcsp offers USART, CAN and ZEROMQ
DRIVERS += "--with-driver-usart=linux --enable-can-socketcan --enable-if-zmqhub"

# Flags for libcsp configuration.
# Currently available flags (sourced from wscript from libcsp-1 github)
# --includes (Add additional include paths, separate with comma)
# --disable-output (Disable CSP output)
# --disable-stlib (Build objects only)
# --enable-rdp (Enable RDP support)
# --enable-rdp-fast-close (Enable fast close of RDP connections)
# --enable-qos (Enable Quality of Service support)
# --enable-promisc (Enable promiscuous support)
# --enable-crc32 (Enable CRC32 support)
# --enable-hmac (Enable HMAC-SHA1 support)
# --enable-xtea (Enable XTEA support)
# --enable-python3-bindings (Enable Python3 bindings)
# --enable-examples (Enable examples)
# --enable-dedup (Enable packet deduplicator)
# --enable-external-debug (Enable external debug API)
# --enable-debug-timestamp (Enable timestamps on debug/log)
# --with-loglevel (Set minimum compile time log level. Must be one of:  + str(valid_loglevel))
# --with-rtable (Set routing table type: \static\ or \cidr\)

FLAGS += ""