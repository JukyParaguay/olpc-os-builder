# Copyright (C) 2012 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.
#
# SELinux causes problems when enabled, e.g. rpm %post scripts fail inside the
# install root.
# http://thread.gmane.org/gmane.linux.redhat.fedora.livecd/4922
#
# If it is enforcing, set it into permissive mode. Anaconda does similarly:
# https://www.redhat.com/archives/anaconda-devel-list/2012-May/msg00315.html 

[ -x /usr/sbin/getenforce ] || exit 0

mode=$(getenforce)
[ "$mode" = "Enforcing" ] || exit 0

setenforce 0
echo
echo "SELinux was found enabled on your system, in enforcing mode."
echo "SELinux is incompatible with olpc-os-builder."
echo
echo "It has been temporarily set to permissive mode to avoid this incompatibility."
echo "It will be re-enabled upon reboot, or alternatively you can re-enable "
echo "it yourself, after olpc-os-builder has completed."
echo
sleep 5
