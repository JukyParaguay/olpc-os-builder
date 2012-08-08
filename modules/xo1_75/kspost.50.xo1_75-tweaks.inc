# enable serial console on ttyS2
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS2.service

# enable serial console for runin too
mkdir -p /etc/systemd/system/runin.target.wants
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/runin.target.wants/serial-getty@ttyS2.service

# FIXME fix dove driver for F18
cat > /usr/share/olpc-utils/xorg.conf.d/xo1.75.conf <<EOF
# This Monitor section is (partially?) unused (#10742)
# as no Screen section is provided, X creates a default monitor.
# but some of these options do seem to be picked up later?
Section "Monitor"
       Identifier       "Monitor0"
       Option  "PanelSize"     "1200x900"

       # FIXME: http://dev.laptop.org/ticket/9331
       DisplaySize 152 114
EndSection

Section "Device"
	Identifier	"Videocard0"
	Option		"UseFBDev"		"true"
	# ShadowFB triggers some segfaults
	Option		"ShadowFB"		"false"
	Option		"fbdev"			"/dev/fb0"
	Driver		"fbdev"
EndSection

Section "Module"
	Disable "glx"
EndSection

Section "Extensions"
	Option "Composite" "False"
EndSection

Section "Screen"
	Identifier "Screen0"
	Device     "Videocard0"
	Monitor    "Monitor0"
EndSection

Section "ServerLayout"
	Identifier "Layout0"
	Screen "Screen0"
	Option "Damage" "False"
EndSection

# Keep these in sync across models
Section "Extensions"
       Option "DPMS" "off"
EndSection
Section "ServerFlags"
	Option "SuspendTime" "0"
	Option "OffTime" "0"
	Option "BlankTime" "0"
	Option "StandbyTime" "0"
	Option "DontZap" "true"
EndSection
EOF
