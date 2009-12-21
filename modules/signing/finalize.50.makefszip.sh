# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
enabled=$(read_config signing make_fs_zip)
[[ "$enabled" == "1" ]] || exit 0

for i in $outputdir/*.zsp; do
#FIXME XO-1 support
	bname=$(basename $i)
	echo "$bname" > $intermediatesdir/version.txt
	cp $outputdir/$i $intermediatesdir/data.img

	zip -n .img:.txt $bname.fs.zip \
		$intermediatesdir/data.img $intermediatesdir/version.txt
done

