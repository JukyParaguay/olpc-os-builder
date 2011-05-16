# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

# create a "shared" intermediates directory which is also available at
# /build_shared from inside the image
mkdir -p $intermediatesdir/shared
echo "mkdir -p \$INSTALL_ROOT/build_shared"
echo "mount --bind $intermediatesdir/shared \$INSTALL_ROOT/build_shared"

