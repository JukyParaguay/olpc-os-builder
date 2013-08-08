. $OOB__shlib

# Put the unsigned kernel/initramfs in zip files and remove the originals
# This avoids duplication for signed builds (where these zips will be signed
# later), where otherwise we would be shipped both signed and unsigned kernels
# and losing some disk space.

suffix=$(read_laptop_model_number)

# Create zip files
create_unsigned_zip() {
	local infile=$1
	local tgt=${2}${suffix}

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

remove_original vmlinuz
remove_original actrd.img
remove_original initrd.img

# symlink actXX to runXX (or the other way) if any of them are missing
create_missing() {
	local dest=${1}${suffix}.zip
	local source=${2}${suffix}.zip

	[ -e $INSTALL_ROOT/boot/$dest ] && return 0
	ln -s $source $INSTALL_ROOT/boot/$dest
}
create_missing actos runos
create_missing actrd runrd
create_missing runos actos
create_missing runrd actrd


create_compat() {
	local compat=${1}.zip
	local dest=${1}${suffix}.zip

	[ -e $INSTALL_ROOT/boot/${dest} ] || return 0
	ln -s $dest $INSTALL_ROOT/boot/$compat
}

# Some old, released firmware versions for XO-1 and XO-1.5 don't support the
# new "versioned" zip file naming scheme. Create symlinks to the old names
# for backwards compatibility.
if [ "$suffix" -lt 2 ]; then
	create_compat actos
	create_compat actrd
	create_compat runos
	create_compat runrd
	create_compat bootfw
fi
