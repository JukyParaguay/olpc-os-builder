# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

# this must be run before the base module creates versioned fs layout

. $OOB__shlib
enabled=$(read_config signing extract)
[[ "$enabled" == "1" ]] || exit 0

buildnr=$(read_buildnr)
tgt=$intermediatesdir/for-signing
outzip=$outputdir/os$buildnr.for-signing.zip
rm -rf $tgt
mkdir -p $tgt

found=0
echo "Extracting content for signing..."
if [ -e "$fsmount/boot/bootfw.zip" ]; then
	cp $fsmount/boot/bootfw.zip $tgt
	found=1
fi

if [ -e "$fsmount/boot/vmlinuz" ]; then
	cp $fsmount/boot/vmlinuz $tgt/data.img
	zip -j -n .img $tgt/runos.zip $tgt/data.img
	rm -f $tgt/data.img
	found=1
fi

if [ -e "$fsmount/boot/initrd.img" ]; then
	cp $fsmount/boot/initrd.img $tgt/data.img
	zip -j -n .img $tgt/runrd.zip $tgt/data.img
	rm -f $tgt/data.img
	found=1
elif [ -e "$fsmount/boot/olpcrd.img" ]; then
	cp $fsmount/boot/olpcrd.img $tgt/data.img
	zip -j -n .img $tgt/runrd.zip $tgt/data.img
	rm -f $tgt/data.img
	found=1
fi

[ "$found" == "1" ] || exit 0

zip -j $outzip $tgt/*

