# basic system packages
chkconfig
kernel
tar
wget
diffutils
rfkill
passwd
bzip2
file
openssh-server
openssh-clients
sudo
vim-minimal
rpm
yum
tree
nano
less
rsyslog

# basic debugging
strace

# OLPC technologies and customization packages
olpc-utils
olpc-netutils
plymouth-theme-olpc
PolicyKit-olpc
ds-backup-client
olpc-powerd
olpc-powerd-dbus

# graphical base environment
olpc-kbdshim
xorg-x11-server-Xorg
xorg-x11-drv-fbdev
xorg-x11-drv-evdev
xorg-x11-drv-keyboard
xorg-x11-drv-mouse
xorg-x11-utils
xorg-x11-xinit
xorg-x11-xauth

# strip fedora trademarks
generic-logos

# bits to set up the livecd while anaconda has been removed here
#isomd5sum

# GSM/CDMA USB modem support, #7172
ModemManager
usb_modeswitch
usb_modeswitch-data

# provides firewall-offline-cmd, needed for kickstart
firewalld
