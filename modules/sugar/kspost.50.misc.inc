mkdir -p /home/olpc/{Activities,Library}
chown olpc: /home/olpc/{Activities,Library}

# temporary workaround for #9823
# FIXME: fix sugar itself then remove this hack
echo 'gtk-font-name = "Sans Serif 7"' >> /usr/share/sugar/data/sugar-100.gtkrc

