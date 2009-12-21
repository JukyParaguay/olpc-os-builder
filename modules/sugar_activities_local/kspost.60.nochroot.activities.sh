# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
actpath=$(read_config sugar_activities_local path)
[ -n "$actpath" -a -d "$actpath" ] || exit 0

for i in "$actpath"/*; do
	if [ "${i:(-4)}" == ".xol" ]; then
        echo "unzip -d \$INSTALL_ROOT/home/olpc/Library -q '$i'"
	else
        echo "unzip -d \$INSTALL_ROOT/home/olpc/Activities -q '$i'"
	fi
done

echo 'chown -R 500:500 $INSTALL_ROOT/home/olpc/{Activities,Library}'

