# Copyright (C) 2010 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

# Get rid of extra locale stuff (#9982)

. $OOB__shlib

langs=$(read_config global langs)
[ -z "$langs" ] && exit 0

# change from format "a,b,c" to format "a|b|c"
langs=${langs//,/|}

echo "localedef --list-archive | grep -v -i -E '$langs' | xargs localedef --delete-from-archive"
echo "mv /usr/lib/locale/locale-archive /usr/lib/locale/locale-archive.tmpl"
echo "/usr/sbin/build-locale-archive"

