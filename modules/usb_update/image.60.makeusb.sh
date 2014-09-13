# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
majver=$(read_config global olpc_version_major)
minver=$(read_config global olpc_version_minor)
relver=$(read_config global olpc_version_release)
versioned_fs=$(read_config base versioned_fs)
buildnr=$(read_buildnr)
suppress=$(read_config usb_update suppress)

if [ "$suppress" = "1" ]; then
	exit 0
fi

if [ "$versioned_fs" != "1" ]; then
	echo "ERROR: usb_upgrade requires base.versioned_fs=1" >&2
	exit 1
fi

echo "Making USB olpc-update image..."
mkisofs -o $outputdir/$(image_name).usb -quiet -cache-inodes -iso-level 4 -publisher "olpc-os-builder" -R -V "$majver.$minver.$relver $buildnr" $fsmount/versions/pristine/*
