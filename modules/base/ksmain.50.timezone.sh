# Copyright (C) 2011 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

tz=$(read_config base timezone)
echo "timezone --utc $tz"

