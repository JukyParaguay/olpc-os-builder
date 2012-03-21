# Copyright (C) 2011 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.
# sign XO-1 ubifs images

. $OOB__shlib
enabled=$(read_config signing make_onu_fs_zip)
[[ "$enabled" == "1" ]] || exit 0

bios_crypto=$(read_config signing bios_crypto_path)
[ -n "$bios_crypto" -a -d "$bios_crypto" ] || exit 0

skey=$(read_config signing skey)

shopt -s nullglob
for i in $outputdir/*.onu; do
	bname=$(basename $i)
	fszip=fs$(read_laptop_model_number).zip
	outfile=$outputdir/$bname.$fszip
	zipfiles="$intermediatesdir/version.txt $intermediatesdir/data.img"

	cp "$i" "$intermediatesdir"/data.img
	echo $(basename $i .onu) > $intermediatesdir/version.txt

	echo "Generating fs.zip for $bname..."
	if [ -n "$skey" ]; then
		echo "Signing..."
		$bios_crypto/build/sig01 sha256 $skey $i > $intermediatesdir/data.sig
		zipfiles="$zipfiles $intermediatesdir/data.sig"
	fi

	zip -j -n .img:.txt:.sig $outfile $zipfiles
	rm -f $zipfiles
done

