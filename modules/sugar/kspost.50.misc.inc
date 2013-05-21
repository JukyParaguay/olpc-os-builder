mkdir -p /home/olpc/{Activities,Library}
chown olpc:olpc /home/olpc/{Activities,Library}

# Remove non-functional squeak menu entry (#10480)
# FIXME: remove when squeak-vm-4.x is adopted, where this change is
# already included.
rm -f /usr/share/applications/squeak.desktop
update-desktop-database &> /dev/null || :
