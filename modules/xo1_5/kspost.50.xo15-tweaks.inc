# enable serial console on ttyS0
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS0.service
