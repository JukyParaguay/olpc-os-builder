# sugar desktop
sugar

# additional sugar packages
libxml2-python
hulahop
pyabiword
gnome-python2-evince
udisks
espeak
etoys
evince-djvu
gtksourceview2
gvfs
libffi
numpy
csound
csound-python
pygame
python-alsaaudio
squeak-vm
# Browse
gnome-python2-gnome
gnash-plugin
totem-mozplugin
# Terminal
vte
# dependencies for Epub support in Read
pywebkitgtk
python-BeautifulSoup
python-lxml
olpc-library
# for Tuxmath activity
SDL_Pango

# for text-to-speech in Read
gstreamer-plugins-espeak

# using a non-upstream updater temporarily (#10641)
sugar-update-control

# explicitly specify dependencies on GNOME's implementation of certain
# components, otherwise the KDE ones will be brought in by default
PolicyKit-gnome
notification-daemon

# standard part of of PyGTK, used by some activities (#11053)
pygtk2-libglade
