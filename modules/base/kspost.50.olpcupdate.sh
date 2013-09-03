# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
versioned_fs=$(read_config base versioned_fs)

if [ "$versioned_fs" = 1 ]; then
	echo systemctl enable olpc-update-query.timer
fi
