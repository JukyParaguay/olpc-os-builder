# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

cache=$cachedir/activities

oIFS=$IFS
IFS=$'\n'
for line in $(env); do
	[[ "${line:0:32}" == "CFG_sugar_activities_extra__url_" ]] || continue
	aurl=${line#*=}
	echo "Downloading from $aurl ..." >&2
	wget --no-verbose --inet4-only -P $cache -N "$aurl"

	outfile=$cache/$(basename "$aurl")
	if [ "${outfile:(-4)}" == ".xol" ]; then
        echo "unzip -d \$INSTALL_ROOT/home/olpc/Library -q '$outfile'"
	else
        echo "unzip -d \$INSTALL_ROOT/home/olpc/Activities -q '$outfile'"
	fi
done
IFS=$oIFS


actpath=$(read_config sugar_activities_extra local_dir)
if [ -n "$actpath" -a -d "$actpath" ]; then
	for i in "$actpath"/*; do
		if [ "${i:(-4)}" == ".xol" ]; then
    	    echo "unzip -d \$INSTALL_ROOT/home/olpc/Library -q '$i'"
		else
    	    echo "unzip -d \$INSTALL_ROOT/home/olpc/Activities -q '$i'"
		fi
	done
fi

echo 'chown -R 500:500 $INSTALL_ROOT/home/olpc/{Activities,Library}'

