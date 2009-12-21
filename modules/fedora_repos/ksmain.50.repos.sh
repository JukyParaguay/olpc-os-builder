# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

fver=$(read_config global fedora_release)

if [[ "$(read_config fedora_repos release)" != "0" ]]; then
	echo "repo --name=fedora --mirrorlist=http://mirrors.fedoraproject.org/mirrorlist?repo=fedora-$fver&arch=i386"
fi

if [[ "$(read_config fedora_repos updates)" != "0" ]]; then
	echo "repo --name=fedora-updates --mirrorlist=http://mirrors.fedoraproject.org/mirrorlist?repo=updates-released-f$fver&arch=i386"
fi

if [[ "$(read_config fedora_repos updates_testing)" != "0" ]]; then
	echo "repo --name=fedora-updates-testing http://mirrors.fedoraproject.org/mirrorlist?repo=updates-testing-f$fver&arch=i386";
fi

exit 0

