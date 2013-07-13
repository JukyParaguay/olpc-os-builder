# Don't try and autospawn pulseaudio (#9470)
sed -i -e 's/; autospawn = yes/autospawn = no/' /etc/pulse/client.conf

# Slim icon theme caches
# these are useful only if disk seeks take a long time.
# rather than remove them completely, we rebuild them as index-only
# (ie, don't duplicate all the icon image data in the cache)
for f in /usr/share/icons/*/icon-theme.cache ; do
	gtk-update-icon-cache -fi --quiet $(dirname $f)
done
