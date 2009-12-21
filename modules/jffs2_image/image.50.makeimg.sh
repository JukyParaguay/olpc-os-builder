# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

buildnr=$(read_buildnr)
tmpimg=$intermediatesdir/jffs2tmp.img
img=$outputdir/os$buildnr.img

mkfs.jffs2 -x rtime -n -e128KiB -r $fsmount -o $tmpimg
sumtool -n -p -e 128KiB -i $tmpimg -o $img
$bindir/crcimg $img

