# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
shopt -s nullglob
content=$(read_config signing add_signed_content)
[ -n "$content" ] || exit 0

echo "Adding signed content..."
# unpack zip file output from the signing laptop and insert into image
signdir=$intermediatesdir/signed-content
rm -rf $signdir
mkdir -p $signdir
unzip $content -d $signdir

pushd $signdir
for sfile in bootfw*.zip runos*.zip runrd*.zip actos*.zip actrd*.zip; do
	cp --remove-destination $sfile $fsmount/boot/$sfile
done
popd

rm -rf $signdir
