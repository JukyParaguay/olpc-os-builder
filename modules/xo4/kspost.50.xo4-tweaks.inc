# enable serial console on ttyS2
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS2.service

# enable serial console for runin too
mkdir -p /etc/systemd/system/runin.target.wants
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/runin.target.wants/serial-getty@ttyS2.service

# FIXME: remove this once XO-4 bluetooth works better
touch /runin/no-bluetooth
echo "blacklist btmrvl_sdio" > /etc/modprobe.d/btmrvl.conf

# fix wifi suspend/resume failure (#12560)
# FIXME: remove once firmware Q7B19 is released
sed -i -e '2ih# 140 d# 35 af!' /boot/olpc.fth
