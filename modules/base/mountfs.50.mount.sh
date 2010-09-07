# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

target_img=$intermediatesdir/rawfs.img
isomnt=$intermediatesdir/mnt-iso
squashmnt=$intermediatesdir/mnt-squashfs

umount $fsmount &>/dev/null || :
umount $squashmnt &>/dev/null || :
umount $isomnt &>/dev/null || :
mkdir -p $isomnt

# find iso
isopath=$outputdir/$(image_name).iso

cleanup() {
	umount $squashmnt &>/dev/null || :
	umount $isomnt &>/dev/null || :
}
trap cleanup SIGINT SIGTERM

# if the config caused us to generate an ISO image with the fs inside (perhaps
# even inside a squashfs), we have to extract it to somewhere where we can
# write to it (i.e. intermediatesdir).
make_iso=$(read_config base make_iso)
if [[ "$make_iso" == "1" ]]; then
	# mount it
	mount -o loop,ro $isopath $isomnt

	# copy out fs image, mounting the squashfs if needed
	if [ -e "$isomnt/LiveOS/squashfs.img" ]; then
		echo "Extracting filesystem image from compressed ISO..."
		mkdir -p $squashmnt
		mount -o loop,ro $isomnt/LiveOS/squashfs.img $squashmnt
		cp $squashmnt/LiveOS/ext3fs.img $target_img
		umount $squashmnt
	else
		echo "Extracting filesystem image from ISO..."
		cp $isomnt/LiveOS/ext3fs.img $target_img
	fi

	umount $isomnt
else
	mv $intermediatesdir/imgcreatefs.img $target_img
fi

echo "Mounting intermediate filesystem image..."
mount -o loop $target_img $fsmount

