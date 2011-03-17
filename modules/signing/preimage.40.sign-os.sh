# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
okey=$(read_config signing okey)
[[ -n "$okey" ]] || exit 0

bios_crypto=$(read_config signing bios_crypto_path)
[ -n "$bios_crypto" -a -d "$bios_crypto" ] || exit 0

if [ -e "$fsmount/boot/vmlinuz" ]; then
	echo "Signing kernel..."
	pushd $bios_crypto/build
	./sign-os.sh $okey $fsmount/boot/vmlinuz $fsmount/boot/runos.zip
	popd
	[ -e $fsmount/boot/actos.zip ] || ln -s runos.zip $fsmount/boot/actos.zip
fi

if [ -e "$fsmount/boot/initrd.img" ]; then
	echo "Signing initramfs..."
	pushd $bios_crypto/build
	./sign-os.sh $okey $fsmount/boot/initrd.img $fsmount/boot/runrd.zip
	popd
fi

if [ -e "$fsmount/boot/actrd.img" ]; then
	echo "Signing activation initramfs..."
	pushd $bios_crypto/build
	$bios_crypto/build/sign-os.sh $okey $fsmount/boot/actrd.img $fsmount/boot/actrd.zip
	popd
fi

# If no separate activation initramfs was provided, assume that the regular
# initramfs also handles activation.
[ -e $fsmount/boot/actrd.zip ] || ln -s runrd.zip $fsmount/boot/actrd.zip

