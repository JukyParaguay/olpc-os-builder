# Copyright (C) 2014 Martin Abente Lahaye - tch@sugarlabs.org
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

url=$(read_config sugar_activity_group url)

echo " 
cat >/usr/share/glib-2.0/schemas/sugar.oob.update.gschema.override <<EOF
[org.sugarlabs.update]
backend='microformat.MicroformatUpdater'
microformat-update-url='${url}'
auto-update-frequency=1
EOF
/usr/bin/glib-compile-schemas /usr/share/glib-2.0/schemas"
