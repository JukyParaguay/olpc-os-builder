# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

add=$(read_config sugar favorites_view_add)
del=$(read_config sugar favorites_view_del)

if [[ -n "$add" ]]; then
	oIFS=$IFS
	IFS=$'\n\t, '
	for activity in $add; do
		echo "echo $activity >> /usr/share/sugar/data/activities.defaults"
	done
	IFS=$oIFS
fi

if [[ -n "$del" ]]; then
	oIFS=$IFS
	IFS=$'\n\t, '
	for activity in $del; do
		echo "sed -i -e '/^$activity$/d' /usr/share/sugar/data/activities.defaults"
	done
	IFS=$oIFS
fi

