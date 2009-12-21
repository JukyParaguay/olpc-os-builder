# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
content=$(read_config signing add_signed_content)
[ -n "$content" ] || exit 0

echo "Adding signed content..."
# unpack zip file output from the signing laptop and insert into image
signdir=$intermediatesdir/signed-content
rm -rf $signdir
mkdir -p $signdir
unzip $content -d $signdir
for sfile in bootfw.zip runos.zip runrd.zip actos.zip actrd.zip; do
	[ -e $signdir/$sfile ] && cp $signdir/$sfile $fsmount/boot/$sfile
done

rm -rf $signdir

# symlink actXX to runXX (or the other way) if any of them are missing
[ -e $fsmount/boot/actos.zip ] || ln -s runos.zip $fsmount/boot/actos.zip
[ -e $fsmount/boot/actrd.zip ] || ln -s runrd.zip $fsmount/boot/actrd.zip
[ -e $fsmount/boot/runos.zip ] || ln -s actos.zip $fsmount/boot/runos.zip
[ -e $fsmount/boot/runrd.zip ] || ln -s actrd.zip $fsmount/boot/runrd.zip

