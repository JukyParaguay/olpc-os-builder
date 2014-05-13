cat >/usr/share/glib-2.0/schemas/sugar.oob.gschema.override <<EOF
[org.sugarlabs]
show-logout=false

[org.sugarlabs.font]
default-size=7

[org.sugarlabs.power]
automatic=true

[org.sugarlabs.collaboration]
jabber-server=''

[org.sugarlabs.user]
default-nick='disabled'
EOF
/usr/bin/glib-compile-schemas /usr/share/glib-2.0/schemas

# set font size in gconf for gtk2 activities
gconftool-2 --direct --config-source xml:readwrite:/etc/gconf/gconf.xml.defaults --type float --set /desktop/sugar/font/default_size 7
