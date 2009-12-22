# permanently set %__dbi_cdb rpm macro to work around jffs2's lack of writeable mmap()
mkdir -p /etc/rpm
echo "%__dbi_cdb create private nommap" > /etc/rpm/macros.rpmdb

# FIXME: old olpc.fth looks for olpcrd.img, but we now use initrd.img
# any nicer way to fix this?
[ -e "/boot/olpcrd.img" ] || ln -s initrd.img /boot/olpcrd.img

# FIXME: really need to teach dracut about mtd devices and stop using mtdblock
# what's more, olpc.fth really shouldnt specify a root device
sed -i -e 's:root=mtd0:root=/dev/mtdblock0:g' /boot/olpc.fth

# FIXME: need an OFW release that runs smbios by default
sed -i -e 's/^olpc-fth-boot-me/setup-smbios olpc-fth-boot-me/' /boot/olpc.fth

