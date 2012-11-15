# enable serial console on ttyS2
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS2.service

# enable serial console for runin too
mkdir -p /etc/systemd/system/runin.target.wants
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/runin.target.wants/serial-getty@ttyS2.service

# FIXME: remove this once #12285 diagnosis is complete
sed -i -e 's/- 200/- 400/g' /runin/runin-memory

# FIXME: remove this once XO-4 suspend works better
touch /runin/no-suspend
