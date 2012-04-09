# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.
# Based on debxo and http://wiki.laptop.org/go/UBIFS_initial_experiments

. $OOB__shlib

buildnr=$(read_buildnr)
versioned_fs=$(read_config base versioned_fs)

compr_type=$(read_config ubifs_image compression_type)
reserved=$(read_config ubifs_image reserved)

BOOT=$intermediatesdir/jffs2-boot
ROOT=$intermediatesdir/ubifs-root
boot_tmp_img=$intermediatesdir/boot_tmp.img
boot_img=$intermediatesdir/boot.img
root_tmp_img=$intermediatesdir/root_tmp.img
root_img=$intermediatesdir/root.img
ubinize_cfg=$intermediatesdir/ubinize.cfg

output_img=$(image_name).uim # uim = UBIFS image
output_script=$(image_name).onu # onu = ofw nand update
output_img_path=$outputdir/$output_img
output_script_path=$outputdir/$output_script

gen_eblocks()
{
	local input=$1
	local eblocks=$((`stat --printf "%s\n" ${input}` / (128*1024)))
	for b in $(seq 0 $(($eblocks - 1))); do
		local sha=$(dd status=noxfer bs=128KiB skip=$b count=1 if=${input} 2>/dev/null \
				| sha256sum | cut -d\  -f1)
		echo "eblock: `printf '%x' $b` sha256 $sha"
	done
}

mkdir -p $BOOT
mkdir -p $ROOT

echo "Copying image contents..."
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
	tgt=$BOOT/boot-versions/$buildnr
	mkdir -p $tgt
	ln -s boot-versions/$buildnr $BOOT/boot
	ln -s boot/alt $BOOT/boot-alt
	cp -ar $ROOT/versions/pristine/$buildnr/boot/* $tgt
else
	cp -ar $ROOT/boot/* $BOOT
	ln -s . $BOOT/boot
fi

echo "Making JFFS2 boot image..."
mkfs.jffs2 -x rtime -n -e128KiB -r $BOOT -o $boot_tmp_img
sumtool -n -p -e 128KiB -i $boot_tmp_img -o $boot_img

[ -n "$compr_type" ] && compr_type="-x $compr_type"

echo "Making UBIFS root image..."
mkfs.ubifs -m 2KiB -e 124KiB -c 7849 $compr_type -R $reserved -d $ROOT -o $root_tmp_img

echo "Ubinizing root image..."
cat > $ubinize_cfg <<EOF
[rootfs]
mode=ubi
image=$root_tmp_img
vol_id=0
vol_type=dynamic
vol_name=rootfs
vol_flags=autoresize
EOF
ubinize -o $root_img -m 2KiB -p 128KiB -s 2KiB $ubinize_cfg

echo "Generating partition script..."
cat > $output_script_path <<EOF
data:  $output_img
erase-all
partitions:  boot c0  system -1
set-partition: boot
mark-pending: 0
EOF

gen_eblocks $boot_img >> $output_script_path

cat >> $output_script_path <<EOF
cleanmarkers
mark-complete: 0
set-partition: system
mark-pending: 0
EOF

gen_eblocks $root_img >> $output_script_path

echo "mark-complete: 0" >> $output_script_path

echo "Create final NAND image..."
cat $boot_img $root_img > $output_img_path

pushd $outputdir >/dev/null
md5sum $output_img > $output_img_path.md5
md5sum $output_script > $output_script_path.md5
popd >/dev/null
