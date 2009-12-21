# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
umount $fsmount &>/dev/null || :
umount $intermediatesdir/mnt-root &>/dev/null || :
umount $intermediatesdir/mnt-boot &>/dev/null || :
umount $intermediatesdir/mnt-squashfs &>/dev/null || :
umount $intermediatesdir/mnt-iso &>/dev/null || :

