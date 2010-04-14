# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

server=$(read_config oats_cfg server)
ignore_xs=$(read_config oats_cfg ignore_xs)

if [ "$ignore_xs" = "1" ]; then
	echo "touch /etc/oats-ignore-xs"
fi

if [ -n "$server" ]; then
	echo "echo '$server' > /etc/oats-server"
fi

