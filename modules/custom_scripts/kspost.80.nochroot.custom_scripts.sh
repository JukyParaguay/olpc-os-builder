# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

find_option_values scripts custom_scripts custom_script
for script in "${scripts[@]}"; do
	echo "echo 'Executing custom script $script'"
	echo "export oob_config_dir=\"$oob_config_dir\""
	echo "[ -x \"$script\" ] && \"$script\" || bash \"$script\""
done
