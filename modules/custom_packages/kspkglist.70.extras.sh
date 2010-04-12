# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

add=$(read_config custom_packages add_packages)
del=$(read_config custom_packages del_packages)

if [[ -n "$add" ]]; then
	oIFS=$IFS
	IFS=$'\n\t, '
	for pkg in $add; do
		echo "$pkg"
	done
	IFS=$oIFS
fi

if [[ -n "$del" ]]; then
	oIFS=$IFS
	IFS=$'\n\t, '
	for pkg in $del; do
		echo "-$pkg"
	done
	IFS=$oIFS
fi

