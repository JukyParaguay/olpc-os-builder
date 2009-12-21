# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
buildnr=$(read_buildnr)
path=$(read_config buildnr_from_file path)

[ -n "$path" ] && echo -n $buildnr > $path

