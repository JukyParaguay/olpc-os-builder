# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

# complementary to kspost.05.mount_shared.nochroot.sh

echo "umount \$INSTALL_ROOT/build_shared"
echo "rmdir \$INSTALL_ROOT/build_shared"

