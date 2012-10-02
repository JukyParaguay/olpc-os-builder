# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
okey=$(read_config signing okey)
[[ -n "$okey" ]] || exit 0

bios_crypto=$(read_config signing bios_crypto_path)
[ -n "$bios_crypto" -a -d "$bios_crypto" ] || exit 0

sign_os() {
	local path=$(find ${fsmount}/boot -type f -name "${1}*.zip" -print -quit)
	[ -z "$path" ] && return

	pushd $bios_crypto/build
	unzip "$path"
	mv data.img tmp.img

	rm -f $path
	./sign-os.sh $okey tmp.img $path

	rm -f tmp.img
	popd
}

echo "Signing initramfs/kernel..."
sign_os runos
sign_os actos
sign_os runrd
sign_os actrd
