# Copyright (C) 2012 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

default_lang=$(read_config base default_language)
default_kbd_model=$(read_config base default_kbd_model)
default_kbd_variant=$(read_config base default_kbd_variant)
default_kbd_layout=$(read_config base default_kbd_layout)
[ -z "$default_lang" ] && [ -z "$default_kbd_model" ] && \
	[ -z "$default_kbd_variant" ] && [ -z "$default_kbd_layout" ] && \
	exit 0

echo "mkdir -p /etc/olpc-configure"
[ -n "$default_lang" ] && \
	echo "echo '$default_lang' > /etc/olpc-configure/default-language"
[ -n "$default_kbd_model" ] && \
	echo "echo '$default_kbd_model' > /etc/olpc-configure/default-kbd-model"
[ -n "$default_kbd_variant" ] && \
	echo "echo '$default_kbd_variant' > /etc/olpc-configure/default-kbd-variant"
[ -n "$default_kbd_layout" ] && \
	echo "echo '$default_kbd_layout' > /etc/olpc-configure/default-kbd-layout"
