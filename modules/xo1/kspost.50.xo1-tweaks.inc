# FIXME: old olpc.fth looks for olpcrd.img, but we now use initrd.img
# any nicer way to fix this?
[ -e "/boot/olpcrd.img" ] || ln -s initrd.img /boot/olpcrd.img

# olpc.fth really shouldnt specify a root device or filesystem, the initramfs
# figures this out (like in the signed boot path).
# FIXME: fix bootfw package
sed -i -e 's:root=mtd0 rootfstype=jffs2::g' /boot/olpc.fth

# do not show the Sugar default ad-hoc networks on the XO-1
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type bool --set /desktop/sugar/network/adhoc false

# enable serial console on ttyS0
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS0.service
