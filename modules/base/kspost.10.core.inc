# limit tmpfs mounts to 5%
cat >> /etc/fstab <<EOF
/tmp            /tmp            tmpfs         rw,size=50m 0 0
vartmp          /var/tmp        tmpfs         rw,size=50m 0 0
varlog          /var/log        tmpfs         rw,size=20m 0 0
EOF
sed -i -e '/\/dev\/shm/ s/ defaults / defaults,size=50m /' /etc/fstab

mkdir /bootpart

# create olpc user
# Provide access to ttyUSB nodes (#8434)
/usr/sbin/useradd -m -c "olpc user" -G audio,wheel,uucp,video,dialout,lock olpc
/usr/bin/passwd -d olpc

# make sure to own home directory and relax permissions a little
chown olpc:olpc /home/olpc
chmod 755 /home/olpc

# move /root to /home/root (#6700)
mv /root /home/root
ln -sT home/root /root

# allow sudo for olpc user
echo "%wheel ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Only allow su access to those in the wheel group (#5537)
sed -i -e '1,6s/^#auth/auth/' /etc/pam.d/su

# Here we deal with a conflict between kickstart and olpc-configure
# olpc-configure creates /etc/sysconfig/keyboard based on mfg data on first
# boot, but kickstart also creates this file for the initial image.
# therefore a live system will always be different from any OS image built
# here :(
# solution: don't ship this file in the image, then it won't get checked
rm -f /etc/sysconfig/keyboard

# A similarly sticky situation is encountered with /etc/sysconfig/network
# We must program our hostname in this file to keep NetworkManager happy
# (#10852), but modifying this file which is ordinarily shipped with the image
# would break pristine-ness. We cannot put this file in statetab for storage
# in the persistent overlay, because /etc/rc.d/rc.sysinit reads this file in
# early boot, before statetab mounts have been put in place (#11060).
# So we resort to removing the file from the image and writing it in
# olpc-configure on first boot.
rm -f /etc/sysconfig/network

cat >/etc/sysconfig/i18n <<__EOF__
# LANG=C considerably speeds up boot (measured 5sec) -- bernie
LANG=C
SYSFONT=sun12x22
__EOF__

# OLPC custom VT layout and serial console setup (#9517, #10354)
sed -i -e 's:/sbin/agetty.*:/sbin/agetty --login-pause --autologin root --noclear %I:' \
	/lib/systemd/system/getty\@.service
sed -i -e 's:/sbin/agetty.*:/sbin/agetty -L -l /bin/bash -w -n %I 115200 vt100:' \
	/lib/systemd/system/serial-getty\@.service

# Enable tmpfs mounts dictated by rwtab (#9637)
mkdir -p /security/state
sed -i -e 's/TEMPORARY_STATE=no/TEMPORARY_STATE=yes/' \
	-e 's:STATE_MOUNT=.*:STATE_MOUNT=/security/state:' \
	/etc/sysconfig/readonly-root

# Remove resolv.conf from rwtab so that it can be updated atomically (#2748)
sed -i -e "/resolv.conf/d" /etc/rwtab

# also remove /tmp, /var/log and /var/tmp from rwtab, since we put them in fstab
sed -i -e '/\t\/tmp/d' /etc/rwtab
sed -i -e '/\t\/var\/tmp/d' /etc/rwtab
sed -i -e '/\t\/var\/log/d' /etc/rwtab

# remove entries from rwtab that we put it in statetab below
sed -i -e '/\t\/var\/lib\/random-seed/d' /etc/rwtab
sed -i -e '/\t\/var\/lib\/dbus/d' /etc/rwtab

# ensure temporary state directory doesn't get too fat (#9636)
sed -i -e 's/RW_OPTIONS=/RW_OPTIONS="-o size=1M -o nr_inodes=1024"/' /etc/sysconfig/readonly-root

# Things to store separately in persistent storage
# This means these files can be writable at runtime without breaking the
# pristine-ness of /versions/pristine. It also means they are retained
# over upgrades.
cat >/etc/statetab.d/olpc <<EOF
/etc/ssh
/etc/sysconfig/i18n
/etc/hosts
/etc/timezone
/var/lib/dbus
/var/lib/random-seed
/etc/NetworkManager/system-connections
EOF

# make sure statetab is set up before recording a random seed during boot
# https://bugzilla.redhat.com/show_bug.cgi?id=808907
sed -i -e '/Before=/ s/$/ systemd-random-seed-load.service/' /lib/systemd/system/fedora-readonly.service

# systemd stuff that is not interesting for us
# random-seed-save: we are interested in saving a random seed, but
# systemd actually does that during boot at systemd-random-seed-load time.
# that is sufficient for our needs, and works around the shutdown issue
# described at https://bugzilla.redhat.com/show_bug.cgi?id=808907#c7
for i in fedora-loadmodules mdmonitor-takeover systemd-readahead-collect \
		fedora-wait-storage fedora-storage-init fedora-storage-init-late \
		fedora-autorelabel fedora-autorelabel-mark systemd-random-seed-save; do
	ln -s /dev/null /etc/systemd/system/$i.service
done

#prefdm tweaks
#sed -i -e 's/respawn limit 10/respawn limit 3/' /etc/init/prefdm.conf

#cat >>/etc/init/prefdm.conf <<EOF
# make sure dcon is unfrozen if something goes wrong.
#post-stop script
#  echo 0 > /sys/devices/platform/dcon/freeze
#end script
#EOF

# OLPC CA certificate (#9624)
# this is used by wget, but the Web activity uses cert8.db in its own
# installation (which is then copied into user's profile dir)
# FIXME: move this into olpc-utils - it can be installed into
# /etc/pki/tls/certs and then it can regenerate cert.pem during %post
cat >> /etc/pki/tls/cert.pem <<EOF

-----BEGIN CERTIFICATE-----
MIIHPDCCBSSgAwIBAgIJANxRsiqV2zHqMA0GCSqGSIb3DQEBBQUAMIHEMTowOAYD
VQQDEzFPbmUgTGFwdG9wIFBlciBDaGlsZCBSb290IENlcnRpZmljYXRpb24gQXV0
aG9yaXR5MQswCQYDVQQGEwJVUzEvMC0GA1UEChMmT25lIExhcHRvcCBQZXIgQ2hp
bGQgQXNzb2NpYXRpb24sIEluYy4xFjAUBgNVBAgTDU1hc3NhY2h1c2V0dHMxEjAQ
BgNVBAcTCUNhbWJyaWRnZTEcMBoGCSqGSIb3DQEJARYNY2FAbGFwdG9wLm9yZzAe
Fw0wNzExMjMxODM5NDVaFw0xMjExMjIxODM5NDVaMIHEMTowOAYDVQQDEzFPbmUg
TGFwdG9wIFBlciBDaGlsZCBSb290IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MQsw
CQYDVQQGEwJVUzEvMC0GA1UEChMmT25lIExhcHRvcCBQZXIgQ2hpbGQgQXNzb2Np
YXRpb24sIEluYy4xFjAUBgNVBAgTDU1hc3NhY2h1c2V0dHMxEjAQBgNVBAcTCUNh
bWJyaWRnZTEcMBoGCSqGSIb3DQEJARYNY2FAbGFwdG9wLm9yZzCCAiIwDQYJKoZI
hvcNAQEBBQADggIPADCCAgoCggIBAKN96sjTagQ5G8Piy5RumOXH9x/4U9fAmFhX
+eF7W4r57W/rqjuG8lyPbLOxmm8WP9pLH5wfksKforQSzF36DBe7RxoRTEk6UT7i
zz/KJEiVCYNeLySOpfNdBOETeAoun8xnaw4w7Ng55YMaMx3Tlm6yQ1NDWnbbhPkh
RvbR26SDsWp/AGFpHkEwQcnQFg/yNn6uBlyN/6hEkFqhl+jJsvizKGSkGI1DdmHy
cMchMb4NFQZcB4j/dqe0nOYfn9nb5UXTucUF4y3PDo3QGrYoZ1+a6n+ZwIWlDDCf
N1+Kp0K0G8MECeVYwc0ydnCSXOzeLF2p1Q+IeCH7e3BIVgltEYQgFWKHufnqvwiI
JVYvbY1kiv7KtTgBgsYj8ciis+X0ULcsDcfZSKcYXwQBJ9jA2Qwim8lOCZXx+rCZ
nRLf2oKShs6TVW6uoHgnNq2p3dzzQpzv5miJEB8JlZRg+O8JxBC593rTBq3SuRH2
J4N5nHAI0PaKoirfJ7wRKzpECBNtQLzkRaeyPQUO5VrivCouOqwEUILph0toiM6v
ZcLfvBLP1JLY2CaPMhGlIBv1Bk0/XfOJF+3CXiRrQ7jLcBYrySJawmTMU4B2k3y6
aBPyP4xx/FrMdi/0NxDIV+DDImButZmv6QxOw6MEFKOl3NjnyqsNgW8EZeTGBcUD
YbYtwyJFAgMBAAGjggEtMIIBKTAdBgNVHQ4EFgQU91un4/npByzkHrrIxahc6Gdo
YXkwgfkGA1UdIwSB8TCB7oAU91un4/npByzkHrrIxahc6GdoYXmhgcqkgccwgcQx
OjA4BgNVBAMTMU9uZSBMYXB0b3AgUGVyIENoaWxkIFJvb3QgQ2VydGlmaWNhdGlv
biBBdXRob3JpdHkxCzAJBgNVBAYTAlVTMS8wLQYDVQQKEyZPbmUgTGFwdG9wIFBl
ciBDaGlsZCBBc3NvY2lhdGlvbiwgSW5jLjEWMBQGA1UECBMNTWFzc2FjaHVzZXR0
czESMBAGA1UEBxMJQ2FtYnJpZGdlMRwwGgYJKoZIhvcNAQkBFg1jYUBsYXB0b3Au
b3JnggkA3FGyKpXbMeowDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOCAgEA
ARhXvy/LncRXS6p6omskGSKR0KzJdZIYyGTrc+zQFabBArwULxVJyrCPNoO4DGRr
sqIJ8U0rkPMQPMhgZuaxgXq7cd6CF/+v/A+IWvM2ocxnmLJ+KOhqiKkRFms6M5mi
N6V46cibyNGYZxT/GL5n98cxnSP1/M+MBgdqa/ddb6eSeZHd1sCY/gFcp0iHCMs/
/EKb0/fSopRNNNbKfNwYPDHTYQr5pL6HZeuJrvhYklfP3+JJXaZFF/Zc4Kex3W7J
fUZjD1uf5b/QMCl4gFelFR8MITskIRm+ufcL/FMelLhDK22FNJKefEP4KcQcEaib
GVsBbZtJv36Gudr1PLvYRZseS6opVNhGlRby7BbKPuLsxP16JGDSD9D2hM5l29SE
qBplJ+XRYXNG50Z7k+ItYRI0n2caEMVCCF39zMVSTIeYLKCoy8Ob1s9j6KNPee/j
8Dggl2qznWin2s2VUnp78Gw42uXb037dXTM8FD+8GPmRIxPHiUuNS5HZ6FjvU4qm
NF8iK9vDtOdUUfugohBD4tGhxsBWc7HWSOYlbT2ZK0M7GQyxe/zZ1ilqdBCo92o8
VBLjX53goylEBj2hyPFmIcnU1gzFol5fFkEQ9FQIAK9gqDmNWZ/+UQcUhNS0ZCiJ
DmGtqUGxKcyVBHKCbGemJaLmiIKya//U7ARDvC7kqfc=
-----END CERTIFICATE-----
EOF

# avahi is a bit annoying in that it modifies /etc/avahi/etc/localtime at
# runtime (copied from /etc/localtime at the start of the init script).
# this breaks contents manifest. olpc-update already has a workaround, but
# if we can fix it here (by excluding this file from the image and hence the
# manifest) then we'll be able to see a successful olpc-contents-verify
# after first boot (see #9839)
#
# this copy can probably be removed too, see
# https://bugzilla.redhat.com/show_bug.cgi?id=546215
rm -f /etc/avahi/etc/localtime

# kill a load of non-interesting anacron tasks (#10247)
chmod -x /etc/cron.daily/logrotate

# Fedora's initscripts packages writes /etc/adjtime without a 3rd line,
# which makes hwclock assume that the hardware clock has local time.
# this will be fixed in util-linux-2.20, where it will default to UTC.
# this hack can be removed when util-linux-2.20 is included. (#10605)
echo UTC >> /etc/adjtime

# not used, takes up a little space
rm -rf /boot/grub

# files with 000 permissions can't be served by updates.laptop.org (#10843)
find / -xdev -type f -perm 000 -exec chmod 400 {} +

# yumdb's from_repo_timestamp and from_repo_revision files change on almost
# every build, even if the packages installed aren't changed. this creates
# needless olpc-update delta. remove these files, which (according to the yum
# source code as of time of writing) are not utilised.
find /var/lib/yum/yumdb -type f -name 'from_repo_revision' -delete -o -name 'from_repo_timestamp' -delete

# enable sysrq by default, possibly useful for debugging phantom hangs
echo "kernel.sysrq = 1" > /usr/lib/sysctl.d/10-olpc.conf

# disable NetworkManager's rh-ifcfg plugin (#9789)
# this ensures that network configs are stored in
# /etc/NetworkManager/system-connections, which is a path that we can safely
# put in statetab. (the rh-ifcfg path in /etc/sysconfig mixes code with data
# and is hence not appropriate to retain over upgrades)
sed -i -e 's/^plugins=ifcfg-rh$/plugins=keyfile/g' /etc/NetworkManager/NetworkManager.conf

# set default plymouth theme
# we do this with plymouthd.defaults so that plymouth-set-default-theme
# could be run by the user at another point in the build process, ensuring
# that the user preference (if specified) sticks.
sed -i -e 's/Theme=.*/Theme=olpc/g' /usr/share/plymouth/plymouthd.defaults

# disable plymouth-start service: we start plymouthd from the initramfs
# with some special settings
ln -s /dev/null /etc/systemd/system/plymouth-start.service

# apply some special settings in the other plymouth service files until
# we have a better solution.
# https://bugs.freedesktop.org/show_bug.cgi?id=22239
sed -i -e 's/plymouthd --mode=shutdown/plymouthd --mode=shutdown "--kernel-command-line=rhgb plymouth.ignore-serial-consoles"/g' /lib/systemd/system/plymouth-poweroff.service
sed -i -e 's/plymouthd --mode=shutdown/plymouthd --mode=shutdown "--kernel-command-line=rhgb plymouth.ignore-serial-consoles"/g' /lib/systemd/system/plymouth-halt.service
sed -i -e 's/plymouthd --mode=shutdown/plymouthd --mode=shutdown "--kernel-command-line=rhgb plymouth.ignore-serial-consoles"/g' /lib/systemd/system/plymouth-reboot.service

# remove the boot-duration file since it gets changed during boot (#11862)
rm -f /var/lib/plymouth/boot-duration

# call olpc-logrotate when log file hits 1mb (#10075)
sed -i -e 's,/var/log/messages,:omfile:$messages,' \
	-e '/$messages/ i$outchannel messages,/var/log/messages,1048576,/usr/sbin/olpc-logrotate' \
	/etc/rsyslog.conf

# tweak upower behaviour, mostly to ignore lid events (#11815)
sed -i -e 's/EnableWattsUpPro=true/EnableWattsUpPro=false/' \
	-e 's/IgnoreLid=false/IgnoreLid=true/' \
	/etc/UPower/UPower.conf

# Tracker has snuck in and is autostarted by default.
# I don't think we want this.
# https://bugzilla.redhat.com/show_bug.cgi?id=821952
rm -f /etc/xdg/autostart/tracker*.desktop 2>/dev/null

# wtmp updating is racy on shutdown (#11952)
# Disable it, it's not interesting for us
rm -f /var/log/wtmp
ln -s /dev/null /etc/systemd/system/systemd-update-utmp-shutdown.service
