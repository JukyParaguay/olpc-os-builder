# basic system packages
@base
@core
chkconfig
kernel.i586
tar
wget
diffutils
rfkill

# basic debugging
strace

# OLPC technologies and customization packages
olpc-utils
olpc-netutils
olpc-bootanim
olpc-update
PolicyKit-olpc
ds-backup-client
olpc-powerd
bootfw

# graphical base environment
olpc-kbdshim
desktop-backgrounds-basic
openssh-askpass
xorg-x11-server-Xorg
xorg-x11-drv-void
xorg-x11-drv-fbdev
xorg-x11-drv-evdev
xorg-x11-drv-keyboard
xorg-x11-drv-mouse
xorg-x11-drv-cirrus
xorg-x11-utils
xorg-x11-xinit
xorg-x11-xauth

# cron needs sendmail, which is provided by exim by default
# and exim needs perl... so let's use ssmtp, which is much lighter
ssmtp

# save some space
-cracklib-dicts
-nss_db
-acpid
-anacron
-mailcap
-wavpack
-createrepo
-irqbalance
-specspo
-esc
-samba-client
-a2ps
-mpage
-redhat-lsb
-sox
-hplip
-hpijs
-sendmail
-numactl
-isdn4k-utils
-autofs
-efibootmgr
-grub
-device-mapper-multipath
-dmraid
-fprintd-pam
-microcode_ctl
-pam_krb5
-pam_pkcs11
-pcmciautils

# dictionaries are big
-aspell-*
-hunspell-*
-man-pages-*
-words

# smartcards won't really work on the livecd.
-coolkey
-ccid

# duplicate functionality
-pinfo
-vorbis-tools

# lose the compat stuff
-compat*

# scanning takes quite a bit of space :/
-xsane
-xsane-gimp
-sane-backends

# no printing
-system-config-printer
-cups*
-foomatic*
-gutenprint*

# attempt to remove dependencies on perl
-w3m
-logwatch
-lftp
-fbset
-exim
-deltarpm

# remove SELinux stack
-setroubleshoot
-policycoreutils
-policycoreutils-gui
-checkpolicy
-selinux-*
-libselinux-python
-libselinux

# other random stuff
-PackageKit*
-bluez*
-samba*
-*-backgrounds
-compiz-gnome

# strip fedora trademarks
-fedora-release-notes
-fedora-logos
generic-logos

# bits to set up the livecd while anaconda has been removed here
isomd5sum

# GSM/CDMA USB modem support, #7172
usb_modeswitch
usb_modeswitch-data
