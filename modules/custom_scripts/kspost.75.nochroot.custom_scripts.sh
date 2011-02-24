# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

oIFS=$IFS
IFS=$'\n'
for line in $(env); do
	[[ "${line:0:34}" == "CFG_custom_scripts__custom_script_" ]] || continue
	script=${line#*=}
	echo "echo 'Executing custom script $script'"
	echo "[ -x \"$script\" ] && \"$script\" || bash \"$script\""
done
IFS=$oIFS

