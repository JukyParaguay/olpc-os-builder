# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

activities=$(read_config sugar protected_activities)

if [[ -n "$activities" ]]; then
    oIFS=$IFS
    IFS=$'\n\t, '
    for activity in $activities; do
        if [[ -n "$list" ]]; then
            list=$list,"'${activity}'"
        else
            list="'${activity}'"
        fi
    done

echo " 
cat >/usr/share/glib-2.0/schemas/sugar.oob.protected.gschema.override <<EOF
[org.sugarlabs]
protected-activities=[${list}]
EOF
/usr/bin/glib-compile-schemas /usr/share/glib-2.0/schemas"

    IFS=$oIFS
fi
