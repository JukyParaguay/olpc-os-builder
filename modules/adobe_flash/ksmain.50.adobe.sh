# Copyright (C) 2012 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

# Only include the repo when a local plugin hasn't been provided

path=$(read_config adobe_flash plugin_path)
[ -n "$path" ] && exit 0

echo "repo --name=adobe --baseurl=http://linuxdownload.adobe.com/linux/i386/"
