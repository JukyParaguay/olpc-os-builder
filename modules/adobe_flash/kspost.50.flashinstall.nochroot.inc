# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

# Install locally-provided flash plugin
path=$(read_config adobe_flash plugin_path)
[ -n "$path" ] || exit 0

mkdir -p $INSTALL_ROOT/usr/lib/mozilla/plugins
cp -v $path $INSTALL_ROOT/usr/lib/mozilla/plugins/libflashplayer.so
