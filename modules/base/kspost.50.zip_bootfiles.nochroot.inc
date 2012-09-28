# Put the unsigned kernel/initramfs in zip files and remove the originals
# This avoids duplication for signed builds (where these zips will be signed
# later), where otherwise we would be shipped both signed and unsigned kernels
# and losing some disk space.

# Create zip files
create_unsigned_zip() {
	local infile=$1
	local tgt=$2

	cd $INSTALL_ROOT/boot
	[ -e $1 ] || return

	cp $1 data.img
	zip -j -n .img ${tgt}.zip data.img
	rm -f data.img
}

create_unsigned_zip vmlinuz runos
create_unsigned_zip initrd.img runrd
create_unsigned_zip actrd.img actrd

# Remove original copies
remove_original() {
	local name=$1

	cd $INSTALL_ROOT/boot
	[ -e "$name" ] || return

	if [ -L "$name" ]; then
		dest=$(readlink $name)
		rm -rf "$dest"
	fi

	rm -f $name
}

#remove_original vmlinuz
#remove_original actrd.img
#remove_original initrd.img

# symlink actXX to runXX (or the other way) if any of them are missing
[ -e $INSTALL_ROOT/boot/actos.zip ] || ln -s runos.zip $INSTALL_ROOT/boot/actos.zip
[ -e $INSTALL_ROOT/boot/actrd.zip ] || ln -s runrd.zip $INSTALL_ROOT/boot/actrd.zip
[ -e $INSTALL_ROOT/boot/runos.zip ] || ln -s actos.zip $INSTALL_ROOT/boot/runos.zip
[ -e $INSTALL_ROOT/boot/runrd.zip ] || ln -s actrd.zip $INSTALL_ROOT/boot/runrd.zip
