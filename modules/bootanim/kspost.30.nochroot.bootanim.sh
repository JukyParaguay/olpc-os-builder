# Copyright (C) 2011 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

custom_image=$(read_config bootanim custom_image)
theme=$(read_config bootanim theme)

if [ -n "$custom_image" ]; then
	echo "cp $custom_image \$INSTALL_ROOT/usr/share/plymouth/themes/olpc/custom.png"
fi

if [ -n "$theme" ]; then
	echo "/usr/sbin/plymouth-set-default-theme $theme"
fi
