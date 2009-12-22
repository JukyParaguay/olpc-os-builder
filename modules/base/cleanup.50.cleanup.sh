# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
umount $fsmount &>/dev/null || :
umount $intermediatesdir/mnt-root &>/dev/null || :
umount $intermediatesdir/mnt-boot &>/dev/null || :
umount $intermediatesdir/mnt-squashfs &>/dev/null || :
umount $intermediatesdir/mnt-iso &>/dev/null || :

# Sometimes, when image-creator is interrupted, it leaves mounts around.
# clean them up here

# instead of trying to figure out which order to umount the appropriate
# mounts, just take a brute-force approach
for tmp in $(seq 10); do
	oIFS=$IFS
	IFS=$'\n'
	for i in $(</proc/mounts); do
		dev=${i%% *}
		i=${i#* }
		i=${i%% *}
		if [[ "${i:0:18}" == "/var/tmp/imgcreate" ]]; then
			umount $i &>/dev/null || :
		fi
		if [[ "${dev:0:9}" == "/dev/loop" ]]; then
			losetup -d $dev &>/dev/null || :
		fi
	done
	IFS=oIFS
done

