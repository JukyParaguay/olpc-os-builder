# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
wkey=$(read_config signing wkey)
[[ -n "$wkey" ]] || exit 0

bios_crypto=$(read_config signing bios_crypto_path)
[ -n "$bios_crypto" -a -d "$bios_crypto" ] || exit 0

[ -e $fsmount/boot/bootfw.zip ] || exit 0

echo "Signing firmware..."

fwtmp=$intermediatesdir/fw-for-signing
mkdir -p $fwtmp
unzip -d $fwtmp $fsmount/boot/bootfw.zip
mv $fwtmp/data.img $intermediatesdir/fw.rom

outzip=$intermediatesdir/bootfw.zip
rm -f $outzip
pushd $bios_crypto/build
$bios_crypto/build/sign-fw.sh $wkey $intermediatesdir/fw.rom $outzip
popd
mv $outzip $fsmount/boot/

