# Set default fonts: We use smaller fonts on the XO

# Application font
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type string --set /desktop/gnome/interface/font_name "Sans 7"

# Document font
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type string --set /desktop/gnome/interface/document_font_name "Sans 7"

# Desktop font
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type string --set /apps/nautilus/preferences/desktop_font "Sans 7"

# Window title font
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type string --set /apps/metacity/general/titlebar_font "Sans Bold 7"

# Monospace Font
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type string --set /desktop/gnome/interface/monospace_font_name "Monospace 7"

# make GNOME mouse cursor bigger
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type int --set /desktop/gnome/peripherals/mouse/cursor_size 48

# disable screensaver locking
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t bool /apps/gnome-screensaver/lock_enabled false

# disable annoying pop-ups from IM settings daemon (we use our own IM settings mechanism for now)
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t bool /apps/imsettings-applet/notify_on_bubble false

