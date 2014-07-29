mkdir -p /home/olpc/{Activities,Library}
chown olpc:olpc /home/olpc/{Activities,Library}

# Remove non-functional squeak menu entry (#10480)
# FIXME: remove when squeak-vm-4.x is adopted, where this change is
# already included.
rm -f /usr/share/applications/squeak.desktop
update-desktop-database &> /dev/null || :

# Sugar web activities need use webkit1. we need remove this on F20
cat > /etc/X11/xinit/xinitrc.d/sugar_web_f18.sh <<EOF
export SUGAR_USE_WEBKIT1=yes
EOF
