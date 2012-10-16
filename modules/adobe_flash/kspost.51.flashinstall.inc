# Run Adobe install script (basically the point of this module)
# see README for an explanation of why this is necessary
[ -e /usr/lib/flash-plugin/setup ] && /usr/lib/flash-plugin/setup 32

# nspluginwrapper is needed at this time, because libflashplayer links
# against GTK2 but our browser (Browse/Epiphany) is GTK3.
# nspluginwrapper provides a process separation model that avoids fatally
# mixing GTK2/GTK3 symbols in the same process.
#
# Moving to WebKit2 will solve this problem, because it moves plugins into
# their own process. When we move to WebKit2 we will be able to drop
# nspluginwrapper.
#
# Delete all already-wrapped plugins, and only wrap libflashplayer
rm -f /usr/lib/mozilla/plugins-wrapped/nswrapper*.so
/usr/lib/nspluginwrapper/npconfig -n -p nswrapper_32_32 \
	-d /usr/lib/mozilla/plugins-wrapped \
	-i /usr/lib/mozilla/plugins/libflashplayer.so
