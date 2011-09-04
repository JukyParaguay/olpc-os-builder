# add /ofw, can be removed when we move to newer kernels with /proc/device-tree
mkdir /ofw
echo "none            /ofw            promfs		  defaults    0 0" >> /etc/fstab
