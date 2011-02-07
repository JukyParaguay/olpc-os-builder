mkdir -p /home/olpc/{Activities,Library}
chown olpc:olpc /home/olpc/{Activities,Library}

# Remove non-functional squeak menu entry (#10480)
# FIXME: remove when squeak-vm-4.x is adopted, where this change is
# already included.
rm -f /usr/share/applications/squeak.desktop
update-desktop-database &> /dev/null || :

# temporarily enable verbose logging during development cycle (#10661)
sed -i \
	-e 's/#export GABBLE_/export GABBLE_/' \
	-e 's/#export SALUT_/export SALUT_/' \
	-e 's/#export PRESENCESERVICE_DEBUG/export PRESENCESERVICE_DEBUG/' \
	-e 's/#export SUGAR_LOGGER_LEVEL/export SUGAR_LOGGER_LEVEL/' \
	/usr/lib/python2.7/site-packages/sugar3/profile.py
