# sugar desktop
sugar
sugar-base
sugar-toolkit
sugar-toolkit-gtk3

# additional sugar packages
libxml2-python
pyabiword
webkitgtk3
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
gnash-plugin
totem-mozplugin

# Terminal
vte

# dependencies for Epub support in Read
python-BeautifulSoup
olpc-library

# for Tuxmath activity
SDL_Pango

# for Physics
pybox2d

# for text-to-speech in Read
gstreamer-plugins-espeak

# using a non-upstream updater temporarily (#10641)
# sugar-update-control # FIXME f16

# explicitly specify dependencies on GNOME's implementation of certain
# components, otherwise the KDE ones will be brought in by default
PolicyKit-gnome
notification-daemon

# standard part of of PyGTK, used by some activities (#11053)
pygtk2-libglade
