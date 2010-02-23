# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

# make a fs.zip, optionally signed, from a .zsp file

. $OOB__shlib
enabled=$(read_config signing make_zsp_fs_zip)
[[ "$enabled" == "1" ]] || exit 0

bios_crypto=$(read_config signing bios_crypto_path)
skey=$(read_config signing skey)

make_unsigned_zsp()
{
	local bname=$(basename $1)
	local bname_noext=$(basename $1 .zsp)

	echo "Generating unsigned fs.zip for $bname..."

	echo "$bname_noext" > $intermediatesdir/version.txt
	cp $i $intermediatesdir/data.img

	zip -j -n .img:.txt $outputdir/$bname.fs.zip \
		$intermediatesdir/data.img $intermediatesdir/version.txt
	rm -f $intermediatesdir/{data.img,version.txt}
}

make_signed_zsp()
{
	echo "Generating signed fs.zip for $(basename $1)..."
	local outfile=$outputdir/$(basename $1).fs.zip
	pushd $bios_crypto/build
	rm -rf $bios_crypto/build/fs.zip
	$bios_crypto/build/sign-zsp.sh $skey $1
	mv $bios_crypto/build/fs.zip $outfile
	popd
}

shopt -s nullglob
for i in $outputdir/*.zsp; do
	if [ -n "$skey" -a -n "$bios_crypto" -a -d "$bios_crypto" ]; then
		make_signed_zsp $i
	else
		make_unsigned_zsp $i
	fi
done

