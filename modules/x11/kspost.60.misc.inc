# Don't try and autospawn pulseaudio (#9470)
sed -i -e 's/; autospawn = yes/autospawn = no/' /etc/pulse/client.conf

# Slim icon theme caches
# these are useful only if disk seeks take a long time.
# rather than remove them completely, we rebuild them as index-only
# (ie, don't duplicate all the icon image data in the cache)
for f in /usr/share/icons/*/icon-theme.cache ; do
	gtk-update-icon-cache -fi --quiet $(dirname $f)
done

# FIXME: move this to olpc-utils /etc/skel ?
# Pre-setup SCIM for Chinese (Pinyin) input
mkdir -p /home/olpc/.scim

cat > /home/olpc/.scim/config << EOF
/DefaultIMEngineFactory/zh_CN = 29ab338a-5a27-46b8-96cd-abbe86f17132
/DefaultIMEngineFactory/zh_HK = 5da9d4ff-ccdd-45af-b1a5-7bd4ac0aeb5f
/DefaultIMEngineFactory/zh_SG = 05235cfc-43ce-490c-b1b1-c5a2185276ae
/DefaultIMEngineFactory/zh_TW = fcff66b6-4d3e-4cf2-833c-01ef66ac6025
/FrontEnd/IMOpenedByDefault = false
/FrontEnd/OnTheSpot = true
/FrontEnd/SharedInputMethod = true
/FrontEnd/Socket/ConfigReadOnly = false
/FrontEnd/Socket/MaxClients = 512
/FrontEnd/X11/BrokenWchar = true
/FrontEnd/X11/Dynamic = true
/FrontEnd/X11/OnTheSpot = true
/FrontEnd/X11/ServerName = SCIM
/Hotkeys/FrontEnd/NextFactory = ISO_Next_Group
/Hotkeys/FrontEnd/NextFactory/zh_CN = Control+Alt+Down,Shift+Control+KeyRelease+Shift_L,Shift+Control+KeyRelease+Shift_R
/Hotkeys/FrontEnd/NextFactory/zh_HK = Control+Alt+Down,Shift+Control+KeyRelease+Shift_L,Shift+Control+KeyRelease+Shift_R
/Hotkeys/FrontEnd/NextFactory/zh_SG = Control+Alt+Down,Shift+Control+KeyRelease+Shift_L,Shift+Control+KeyRelease+Shift_R
/Hotkeys/FrontEnd/NextFactory/zh_TW = Control+Alt+Down,Shift+Control+KeyRelease+Shift_L,Shift+Control+KeyRelease+Shift_R
/Hotkeys/FrontEnd/Off = 
/Hotkeys/FrontEnd/On = 
/Hotkeys/FrontEnd/PreviousFactory = 
/Hotkeys/FrontEnd/PreviousFactory/zh_CN = Control+Alt+Up,Shift+Control+KeyRelease+Control_L,Shift+Control+KeyRelease+Control_R
/Hotkeys/FrontEnd/PreviousFactory/zh_HK = Control+Alt+Up,Shift+Control+KeyRelease+Control_L,Shift+Control+KeyRelease+Control_R
/Hotkeys/FrontEnd/PreviousFactory/zh_SG = Control+Alt+Up,Shift+Control+KeyRelease+Control_L,Shift+Control+KeyRelease+Control_R
/Hotkeys/FrontEnd/PreviousFactory/zh_TW = Control+Alt+Up,Shift+Control+KeyRelease+Control_L,Shift+Control+KeyRelease+Control_R
/Hotkeys/FrontEnd/ShowFactoryMenu = 
/Hotkeys/FrontEnd/Trigger = Control+space,ISO_Next_Group
/Hotkeys/FrontEnd/ValidKeyMask = Shift+Control+Alt+Meta+Super+Hyper+CapsLock
/IMEngine/RawCode/Locales = default
/Panel/Gtk/Color/ActiveBackground = light sky blue
/Panel/Gtk/Color/ActiveText = black
/Panel/Gtk/Color/NormalBackground = #F7F3F7
/Panel/Gtk/Color/NormalText = black
/Panel/Gtk/DefaultSticked = false
/Panel/Gtk/Font = default
/Panel/Gtk/LookupTableEmbedded = true
/Panel/Gtk/LookupTableVertical = true
/Panel/Gtk/ShowStatusBox = false
/Panel/Gtk/ShowTrayIcon = false
/Panel/Gtk/ToolBar/AlwaysHidden = false
/Panel/Gtk/ToolBar/AlwaysShow = false
/Panel/Gtk/ToolBar/AutoSnap = true
/Panel/Gtk/ToolBar/HideTimeout = 2
/Panel/Gtk/ToolBar/POS_X = -1
/Panel/Gtk/ToolBar/POS_Y = -1
/Panel/Gtk/ToolBar/ShowFactoryIcon = true
/Panel/Gtk/ToolBar/ShowFactoryName = true
/Panel/Gtk/ToolBar/ShowHelpIcon = true
/Panel/Gtk/ToolBar/ShowMenuIcon = true
/Panel/Gtk/ToolBar/ShowPropertyLabel = true
/Panel/Gtk/ToolBar/ShowSetupIcon = true
/Panel/Gtk/ToolBar/ShowStickIcon = false
/UpdateTimeStamp = 1256504419:916046
EOF

cat > /home/olpc/.scim/global << EOF
/DefaultKeyboardLayout = US_Default
/SupportedUnicodeLocales = en_US.UTF-8,zh_CN.UTF-8
EOF

chown -R olpc:olpc /home/olpc/.scim

exit 0

