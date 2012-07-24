# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

cache=$cachedir/activities

find_option_values urls sugar_activities_extra url
for aurl in "${urls[@]}"; do
	echo "Downloading from $aurl ..." >&2
	[ -z "${OOB__cacheonly}" ] && \
		wget --no-verbose --inet4-only -P $cache -N "$aurl"
	install_sugar_bundle $cache/$(basename "$aurl")
done

find_option_values dirs sugar_activities_extra local_dir
for actpath in "${dirs[@]}"; do
	[ -n "$actpath" -a -d "$actpath" ] || continue
	for i in "$actpath"/*; do
		install_sugar_bundle $i
	done
done
