# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

langs=$(read_config osk languages)

[ -z "$langs" ] && exit 0

oIFS=$IFS
IFS=$'\n\t, '
for lang in $langs; do
	output+=", libmaliit-keyboard-plugin.so:${lang}"
done
IFS=$oIFS

output="${output:2}"
echo 'mkdir -p /etc/xdg/maliit.org'
echo 'echo "[maliit]" > /etc/xdg/maliit.org/server.conf'
echo "echo 'onscreen\\enabled=${output}' >> /etc/xdg/maliit.org/server.conf"
