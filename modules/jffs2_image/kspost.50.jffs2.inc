# permanently set %__dbi_cdb rpm macro to work around jffs2's lack of writeable mmap()
mkdir -p /etc/rpm
echo "%__dbi_cdb create private nommap" > /etc/rpm/macros.rpmdb

