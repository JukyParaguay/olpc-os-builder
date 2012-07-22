# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

while IFS= read -r -d '' line; do
	[[ "${line:0:34}" == "CFG_custom_packages__add_packages_" || "${line:0:34}" == "CFG_custom_packages__add_packages=" ]] || continue
	pkgs=${line#*=}
	oIFS=$IFS
	IFS=$'\n\t, '
	for pkg in $pkgs; do
		echo "$pkg"
	done
	IFS=$oIFS
done < <(env --null)

while IFS= read -r -d '' line; do
	[[ "${line:0:34}" == "CFG_custom_packages__del_packages_" || "${line:0:34}" == "CFG_custom_packages__del_packages=" ]] || continue
	pkgs=${line#*=}
	oIFS=$IFS
	IFS=$'\n\t, '
	for pkg in $pkgs; do
		echo "-$pkg"
	done
	IFS=$oIFS
done < <(env --null)
