# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

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
for i in bootfw.zip vmlinuz initrd.img olpcrd.img; do
	path="$fsmount/boot/$i"
	[ -e "$path" ] || continue
	found=1
	cp "$path" $tgt
done

[ "$found" == "1" ] || exit 0

zip $outzip $tgt/*

