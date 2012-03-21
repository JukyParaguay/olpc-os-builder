# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
majver=$(read_config global olpc_version_major)
minver=$(read_config global olpc_version_minor)
relver=$(read_config global olpc_version_release)
fver=$(read_config global fedora_release)
platform=$(read_config global target_platform)
official=$(read_config global official)
custinfo=$(read_config global customization_info)
buildnr=$(read_buildnr)

[ -z "$custinfo" ] && custinfo="customized"
#add leading space
custinfo=" $custinfo"

custstr=
if [[ "$official" != "1" ]]; then
	custstr=",$custinfo"
	custbstr="$custinfo"
fi

buildstr="$majver.$minver.$relver for ${platform}${custstr} (build $buildnr)"

cat <<EOF
# needed for spin debranding
echo "OLPC release $majver (based on Fedora $fver)" > /etc/fedora-release

# this is used by the activity updater
echo "$majver.$minver.$relver" > /etc/olpc-release

sed -i -e "1s/.*/OLPC OS $buildstr/" /etc/issue
cp /etc/issue /etc/issue.net

echo "${buildstr}" > /boot/olpc_build
EOF

