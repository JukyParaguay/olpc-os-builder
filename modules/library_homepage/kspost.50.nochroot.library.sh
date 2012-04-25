# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

path=$(read_config library_homepage path)

if [ -z "$path" ]; then
	echo "ERROR: No library customization path specified"
	exit 1
fi

if [ ! -d "$path" ]; then
	echo "ERROR: library_homepage.path must point to a directory"
	exit 1
fi

# synchronize the files within the path, conserving directory structure
echo "rsync -rlpt \"$path\"/ \"\$INSTALL_ROOT/usr/share/library-common/\""
echo "chmod -R ugo+rx \"\$INSTALL_ROOT/usr/share/library-common/\""
