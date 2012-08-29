find $INSTALL_ROOT/usr/{sbin,bin,lib,libexec} -xdev -type f -and ! -name '*.ko' | while read file; do
	ftype=$(file -b "$file")
	[ "${ftype:0:4}" = "ELF " ] && strip -R .gnu_debugdata "$file" || :
done
exit 0
