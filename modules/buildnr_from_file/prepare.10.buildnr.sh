# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
nr=0

path=$(read_config buildnr_from_file path)
[ -n "$path" -a -e "$path" ] && nr=$(<$path)

# increment and ignore return value. new versions of bash return the new
# value of the variable which is usually non-zero.
(( nr++ )) || :

echo $nr > $intermediatesdir/buildnr

