# enable serial console on ttyS0
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS0.service

# enable serial console for runin too
mkdir -p /etc/systemd/system/runin.target.wants
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/runin.target.wants/serial-getty@ttyS0.service

# Workaround an unknown bug where TamTam sound is crackly on XO-1.5 (#9414)
sed -i -e 's/dmix.rate 48000/dmix.rate 44100/' /usr/share/alsa/alsa.conf
