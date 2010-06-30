# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

idle_suspend_enabled=$(read_config powerd enable_idle_suspend)
if [[ "$idle_suspend_enabled" != 1 ]]; then
	echo "touch /etc/powerd/flags/inhibit-suspend"
fi

