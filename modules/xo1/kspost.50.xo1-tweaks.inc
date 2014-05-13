# do not show the Sugar default ad-hoc networks on the XO-1
cat >/usr/share/glib-2.0/schemas/sugar.oob.xo1.gschema.override <<EOF
[org.sugarlabs.network]
adhoc=false
EOF
/usr/bin/glib-compile-schemas /usr/share/glib-2.0/schemas

# enable serial console on ttyS0
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS0.service
