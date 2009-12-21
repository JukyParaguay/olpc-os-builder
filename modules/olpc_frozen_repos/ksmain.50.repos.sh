# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

tags=$(read_config olpc_frozen_repos mocktags)
IFS=$'\n\t, '
for tag in $tags; do
	echo "repo --name=$tag --baseurl=http://mock.laptop.org/repos/$tag"
done
IFS=$oIFS

