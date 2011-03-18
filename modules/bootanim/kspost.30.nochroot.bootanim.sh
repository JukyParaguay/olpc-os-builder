# Copyright (C) 2011 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

cache=$cachedir/bootanim

STATIC_IMAGES="frame00.565 ul_warning.565"
FILES="$STATIC_IMAGES deltas"

for f in $FILES; do
	echo "cp $cache/$f \$INSTALL_ROOT/usr/share/boot-anim/"
done
echo 'chmod 644 $INSTALL_ROOT/usr/share/boot-anim/*'
