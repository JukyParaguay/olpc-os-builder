# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

# share the intermediates/shared directory at /build_shared inside the
# image build environment.
echo "mkdir -p \$INSTALL_ROOT/build_shared"
echo "mount --bind $shareddir \$INSTALL_ROOT/build_shared"
