# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

activities=$(read_config sugar protected_activities)

if [[ -n "$activities" ]]; then
    oIFS=$IFS
    IFS=$'\n\t, '
    for activity in $activities; do
        if [[ -n "$list" ]]; then
            list=$list','$activity
        else
            list=$activity
        fi
    done
    echo "gconftool-2  --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type list --list-type string --set /desktop/sugar/protected_activities [$list]"
    IFS=$oIFS
fi
