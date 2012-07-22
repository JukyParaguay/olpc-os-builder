# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

find_option_values pkgs_add custom_packages add_packages
for pkgs in "${pkgs_add[@]}"; do
	oIFS=$IFS
	IFS=$'\n\t, '
	for pkg in $pkgs; do
		echo "$pkg"
	done
	IFS=$oIFS
done

find_option_values pkgs_del custom_packages del_packages
for pkgs in "${pkgs_del[@]}"; do
	oIFS=$IFS
	IFS=$'\n\t, '
	for pkg in $pkgs; do
		echo "-$pkg"
	done
	IFS=$oIFS
done
