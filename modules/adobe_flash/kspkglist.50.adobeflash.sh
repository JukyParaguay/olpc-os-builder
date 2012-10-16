# Copyright (C) 2012 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

# Only install the package when a local plugin hasn't been provided
path=$(read_config adobe_flash plugin_path)
[ -n "$path" ] && exit 0

echo "flash-plugin"
