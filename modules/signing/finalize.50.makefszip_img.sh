# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

# sign XO-1 jffs2 images

. $OOB__shlib
enabled=$(read_config signing make_img_fs_zip)
[[ "$enabled" == "1" ]] || exit 0

bios_crypto=$(read_config signing bios_crypto_path)
[ -n "$bios_crypto" -a -d "$bios_crypto" ] || exit 0

skey=$(read_config signing skey)

make_unsigned_img()
{
	local bname=$(basename $1)
	local bname_noext=$(basename $1 .img)

	echo "Generating unsigned fs.zip for $bname..."

	echo "$bname" > $intermediatesdir/data.img
	$bios_crypto/build/hashfs sha256 "$i" >> $intermediatesdir/data.img
	echo $bname_noext > $intermediatesdir/version.txt

	zip -j -n .img:.txt $outputdir/$bname.fs.zip \
		$intermediatesdir/data.img $intermediatesdir/version.txt
	rm -f $intermediatesdir/{data.img,version.txt}
}

make_signed_img()
{
	echo "Generating signed fs.zip for $(basename $1)..."
	local fszip=fs$(read_laptop_model_number).zip
	local outfile=$outputdir/$(basename $1).$fszip
	pushd $bios_crypto/build
	./make-fs.sh --signingkey $skey $1
	mv fs.zip $outfile
	popd
}

shopt -s nullglob
for i in $outputdir/*.img; do
	# skip SD card disk images
	[[ "${i:(-9)}" == ".disk.img" ]] && continue

	if [ -n "$skey" ]; then
		make_signed_img $i
	else
		make_unsigned_img $i
	fi
done

