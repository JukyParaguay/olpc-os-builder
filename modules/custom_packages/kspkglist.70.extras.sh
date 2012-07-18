# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

oIFS=$IFS
IFS=$'\n'
for line in $(env); do
	[[ "${line:0:34}" == "CFG_custom_packages__add_packages_" || "${line}" == "CFG_custom_packages__add_packages" ]] || continue
	pkgs=${line#*=}
	oIFS2=$IFS
	IFS=$'\n\t, '
	for pkg in $pkgs; do
		echo "$pkg"
	done
	IFS=$oIFS2
done

for line in $(env); do
	[[ "${line:0:34}" == "CFG_custom_packages__add_packages_" || "${line}" == "CFG_custom_packages__add_packages" ]] || continue
	pkgs=${line#*=}
	oIFS2=$IFS
	IFS=$'\n\t, '
	for pkg in $pkgs; do
		echo "$pkg"
	done
	IFS=$oIFS2
done
