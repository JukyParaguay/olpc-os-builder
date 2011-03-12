# add /ofw, can be removed when we move to newer kernels with /proc/device-tree
mkdir /ofw
echo "none            /ofw            promfs		  defaults    0 0" >> /etc/fstab

# enable serial console on ttyS0
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS0.service
