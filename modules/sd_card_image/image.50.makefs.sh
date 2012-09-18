# Copyright (C) 2009 One Laptop per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
versioned_fs=$(read_config base versioned_fs)
buildnr=$(read_buildnr)
BLOCK_SIZE=512
ROOT_PARTITION_START_BLOCK=139264
NUM_HEADS=16
NUM_SECTORS_PER_TRACK=62

# FIXME trap signals and cleanup
# FIXME check that traps due to errors are caught
BOOT=$intermediatesdir/mnt-boot
ROOT=$intermediatesdir/mnt-root

umount $BOOT &>/dev/null || :
umount $ROOT &>/dev/null || :
mkdir -p $BOOT
mkdir -p $ROOT

# Automatically determine a size for the output disk image (including root
# and boot partitions).
#
# This is calculated by examining how much space was used in the intermediate
# filesystem image, and by adding a small amount of free space for safety.
auto_size()
{
	local rawfs=$intermediatesdir/rawfs.img
	local edump=$(dumpe2fs "$rawfs")
	local bsize=$(echo "$edump" | grep "^Block size:")
	local bcount=$(echo "$edump" | grep "^Block count:")
	local freeblocks=$(echo "$edump" | grep "^Free blocks:")

	# Remove textual labels, we just want the numbers
	bsize="${bsize##* }"
	bcount="${bcount##* }"
	freeblocks="${freeblocks##* }"

	local usedblocks=$(( bcount - freeblocks ))
	local usedsize=$(( usedblocks * bsize ))

	# In my testing, the new image has about 100mb free even when we try
	# to match the size exactly. So we use the exact size; if we find that
	# we need to add some 'safety' space later, we can add it.
	#local newsize=$(( usedsize + (20*1024*1024) ))
	local newsize=$usedsize

	# Increase by size of boot partition
	(( newsize += $ROOT_PARTITION_START_BLOCK * $BLOCK_SIZE ))

	echo $newsize
}

make_image()
{
	local disk_size=$1
	local ext=$2
	[ -z "$ext" ] && ext="zd"

	if [ "$disk_size" = "auto" ]; then
		disk_size=$(auto_size)
	fi

	echo "Making image of size $disk_size"

	echo "Create disk and partitions..."

	local num_blocks=$(($disk_size / $BLOCK_SIZE))
	local num_cylinders=$(($num_blocks / $NUM_HEADS / $NUM_SECTORS_PER_TRACK))
	local image_size=$(($num_cylinders * $NUM_HEADS * $NUM_SECTORS_PER_TRACK * $BLOCK_SIZE))

	local img=$intermediatesdir/$(image_name).$ext.disk.img

	dd if=/dev/zero of=$img bs=$BLOCK_SIZE count=0 seek=$(($image_size / $BLOCK_SIZE))

	/sbin/sfdisk -S 32 -H 32 --force -uS $img <<EOF
8192,131072,83,*
$ROOT_PARTITION_START_BLOCK,,,
EOF

	disk_loop=$(losetup --show --find --partscan $img)
	boot_loop="${disk_loop}p1"
	root_loop="${disk_loop}p2"

	# Work around occasional failure for loop partitions to appear
	# http://marc.info/?l=linux-kernel&m=134271282127702&w=2
	local i=0
	while ! [ -e "$boot_loop" ]; do
		partx -a -v $disk_loop
		sleep 1
		(( ++i ))
		[ $i -ge 10 ] && break
	done

	echo "Create filesystems..."
	mke2fs -O dir_index,^resize_inode -L Boot -F $boot_loop
	mount $boot_loop $BOOT

	mkfs.ext4 -O dir_index,^huge_file -E resize=8G -m1 -L OLPCRoot $root_loop
	tune2fs -o journal_data_ordered $root_loop
	mount $root_loop $ROOT

	echo "Copy in root filesystem..."
	cp -a $fsmount/* $ROOT

	echo "Setup boot partition..."

	# runin testing needs this directory (#9840)
	# this needs to be done during build so that OFW can put files here
	# (e.g. updated tests) before the OS has ever booted
	mkdir -p $BOOT/runin

	# we put /security here as it's used by OFW, and should persist between
	# updates
	mkdir -p $BOOT/security

	# this is where Fedora's statetab tmpfs mount system puts its data.
	# the directory has to be created in advance
	mkdir -p $BOOT/security/state

	if [ "$versioned_fs" = "1" ]; then
		local tgt=$BOOT/boot-versions/$buildnr
		mkdir -p $tgt
		ln -s boot-versions/$buildnr $BOOT/boot
		ln -s boot/alt $BOOT/boot-alt
		cp -ar $ROOT/versions/pristine/$buildnr/boot/* $tgt
	else
		cp -ar $ROOT/boot/* $BOOT
		ln -s . $BOOT/boot
	fi

	umount $ROOT
	umount $BOOT
	losetup -d $disk_loop || :

	# FIXME: any value to running e2fsck now? maybe with -D ?
}


find_option_values sizes sd_card_image size
for val in "${sizes[@]}"; do
	disk_size=${val%,*}
	ext=
	expr index "$vals" ',' &>/dev/null && ext=${vals#*,}
	make_image $disk_size $ext
done

# If no sizes were specified, create an image with automatic size.
[[ ${#sizes[@]} == 0 ]] && make_image auto
