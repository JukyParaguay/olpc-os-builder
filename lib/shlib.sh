# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

set -e

libdir=$OOB__libdir
bindir=$OOB__bindir
builddir=$OOB__builddir
cachedir=$OOB__cachedir
intermediatesdir=$OOB__intermediatesdir
outputdir=$OOB__outputdir
statedir=$OOB__statedir
fsmount=$OOB__fsmount

read_config() {
	local vname="CFG_$1__$2"
	echo ${!vname}
}

read_buildnr() {
	local buildnr_path=$intermediatesdir/buildnr
	if [[ -e $buildnr_path ]]; then
		echo "$(<$buildnr_path)"
	else
		echo "0"
	fi
}

