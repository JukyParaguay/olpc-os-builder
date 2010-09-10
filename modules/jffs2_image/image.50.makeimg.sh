# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

buildnr=$(read_buildnr)
versioned_fs=$(read_config base versioned_fs)

tmpdir=$intermediatesdir/jffs2-fs
tmpimg=$intermediatesdir/jffs2tmp.img
outfile=$(image_name).img
img=$outputdir/$outfile


echo "Copying image contents..."
rm -rf $tmpdir
cp -a $fsmount $tmpdir

if [ "$versioned_fs" = "1" ]; then
	# setup for unpartitioned boot of versioned fs
	mkdir -p $tmpdir/versions/configs/$buildnr
	ln -s versions/boot/current/boot $tmpdir/boot
	ln -s versions/boot/alt/boot $tmpdir/boot-alt
	ln -s configs/$buildnr $tmpdir/versions/boot
	ln -s /versions/pristine/$buildnr $tmpdir/versions/configs/$buildnr/current
fi

echo "Making JFFS2 image..."
mkfs.jffs2 -x rtime -n -e128KiB -r $tmpdir -o $tmpimg

echo "Creating checksums..."
sumtool -n -p -e 128KiB -i $tmpimg -o $img
crcimg $img

pushd $outputdir >/dev/null
md5sum $outfile > $outfile.md5
popd >/dev/null
