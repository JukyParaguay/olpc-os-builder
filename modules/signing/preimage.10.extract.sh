# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

# this must be run before the base module creates versioned fs layout

. $OOB__shlib
enabled=$(read_config signing extract)
[[ "$enabled" == "1" ]] || exit 0

tgt=$intermediatesdir/for-signing
outzip=$outputdir/$(image_name).for-signing.zip
rm -rf $tgt
mkdir -p $tgt

found=0
echo "Extracting content for signing..."

copy_out_file() {
	local name=$1
	local path="$fsmount"/boot/${1}.zip
	[ -f "$path"] || return
	cp $path $tgt
	found=1
}

copy_out bootfw
copy_out runos
copy_out runrd
copy_out actos
copy_out actrd
[ "$found" == "1" ] || exit 0

zip -j $outzip $tgt/*
