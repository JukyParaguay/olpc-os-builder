# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
buildnr=$(read_buildnr)
treetar=$outputdir/os$buildnr.tree.tar.lzma
pkglist=$outputdir/os$buildnr.packages.txt

isopath=$outputdir/os$buildnr.iso

maketree=$(read_config base make_tree_tarball)
if [[ "$maketree" == "1" ]]; then
	echo "Make tree tarball..."
	tar -c -C $fsmount . | lzma -1 > $treetar

	echo "Checksum tree tarball..."
	md5sum $treetar > $treetar.md5
fi

versioned_fs=$(read_config base versioned_fs)
if [ "$versioned_fs" = "1" ]; then
	chroot_path=$fsmount/versions/pristine/$buildnr
else
	chroot_path=$fsmount
fi

chroot $chroot_path /bin/rpm -qa | sort > $pkglist

