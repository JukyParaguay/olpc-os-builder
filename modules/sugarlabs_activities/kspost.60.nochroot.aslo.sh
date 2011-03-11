# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

sugarver=$(read_config sugarlabs_activities sugar_version)
experimental=$(read_config sugarlabs_activities experimental)
activities=$(read_config sugarlabs_activities activities)
cache=$cachedir/activities
mkdir -p $cache

oIFS=$IFS
IFS=$'\n\t, '
for id in $activities; do
	qurl="http://activities.sugarlabs.org/services/update-aslo.php?id=$id"
	[ -n "$sugarver" ] && qurl="${qurl}&appVersion=${sugarver}"
	[ "$experimental" = "1" ] && qurl="${qurl}&experimental=1"

	echo "Examining $qurl ..." >&2
	aurl=$(wget --inet4-only -q -O- "$qurl" | grep updateLink | sed -e 's/[[:space:]]*<[^>]*>//g')
	if [ -z "$aurl" ]; then
		echo "ERROR: Could not find download URL for $id" >&2
		exit 1
	fi

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

echo 'chown -R 500:500 $INSTALL_ROOT/home/olpc/{Activities,Library}'

