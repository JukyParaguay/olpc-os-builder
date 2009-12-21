# Sugar font
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type float --set /desktop/sugar/font/default_size 7

# disable logout button in Sugar
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t bool /desktop/sugar/show_logout false

# set sugar power management on
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t bool /desktop/sugar/power/automatic true

# set empty jabber server so that no gabble connection is attempted by default
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type string --set /desktop/sugar/collaboration/jabber_server ''


