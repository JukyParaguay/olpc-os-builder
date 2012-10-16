# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

# Install yum repo entry for Adobe repo when we get the package from there
path=$(read_config adobe_flash plugin_path)
[ -n "$path" ] && exit 0

cat <<EOF >$INSTALL_ROOT/etc/yum.repos.d/adobe-linux-i386.repo
[adobe-linux-i386]
name=Adobe Systems Incorporated
baseurl=http://linuxdownload.adobe.com/linux/i386/
enabled=1
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-adobe-linux
EOF
