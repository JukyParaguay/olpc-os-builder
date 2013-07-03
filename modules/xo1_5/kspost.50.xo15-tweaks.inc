# enable serial console on ttyS0
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS0.service

# enable serial console for runin too
# mkdir -p /etc/systemd/system/runin.target.wants
# ln -sf /lib/systemd/system/serial-getty@.service \
#	/etc/systemd/system/runin.target.wants/serial-getty@ttyS0.service

# enable openchrome driver
cat > /usr/share/olpc-utils/xorg.conf.d/xo1.5.conf << 'EOF'
Section "Monitor"
	Identifier   "Monitor0"
	VendorName   "Monitor Vendor"
	ModelName    "Monitor Model"

	# FIXME: http://dev.laptop.org/ticket/9331
	DisplaySize 152 114

	HorizSync 20-90
	VertRefresh 49-51 
	Option "DPMS" "off"
EndSection

Section "Device"
        ### Available Driver options are:-
        ### Values: <i>: integer, <f>: float, <bool>: "True"/"False",
        ### <string>: "String", <freq>: "<f> Hz/kHz/MHz"
        ### [arg]: arg optional
        Option     "ForceLCD"    "yes"       	# [<bool>]
        Option     "ForcePanel"    "yes"       	# [<bool>]
        #Option     "NoAccel"            	# [<bool>]
        #Option     "AccelMethod"        	# <str>
        #Option     "ExaNoComposite"     	# [<bool>]
        #Option     "ExaScratchSize"     	# <i>
        Option     "SWCursor"  "yes"
        #Option     "ShadowFB"           	# [<bool>]
        #Option     "Rotate"             	# [<str>]
        #Option     "RotateType"         	# [<str>]
        #Option     "Inverse"            	# [<bool>]
        #Option     "VideoRAM"           	# <i>
        #Option     "VideoRAM1"          	# <i>
        #Option     "TwoLCD"             	# [<bool>]
        #Option     "CRTPort"            	# [<str>]
        Option     "ActiveDevice"  "LCD"     	# [<str>]
        #Option     "DVIPort"            	# [<str>]
        #Option     "DVIDualEdge"        	# [<str>]
        #Option     "DVIBusWidth"        	# [<str>]
        #Option     "Simultaneous"       	# [<bool>]
        #Option     "LCDPort"            	# [<str>]
        #Option     "LCDBusWidth"        	# [<str>]
        #Option     "Center"             	# [<bool>]
        #Option     "PanelID"            	# <i>
	Option	"PanelSize" "1200x900"
        #Option     "TVPort"             	# [<str>]
        #Option     "TVDotCrawl"         	# [<bool>]
        #Option     "TV2DotCrawl"        	# [<bool>]
        #Option     "TVProgressive"      	# [<bool>]
        #Option     "TV2Progressive"     	# [<bool>]
        #Option     "TVType"             	# [<str>]
        #Option     "TV2Type"            	# [<str>]
        #Option     "TVOutput"           	# [<str>]
        #Option     "TV2Output"          	# [<str>]
        #Option     "TVVScan"            	# [<str>]
        #Option     "TVHScale"           	# <i>
        #Option     "TVVScale"           	# <i>
        #Option     "TV2HScale"          	# <i>
        #Option     "TV2VScale"          	# <i>
        #Option     "TVEncoder"          	# [<str>]
        #Option     "TVFlickerFilter"    	# <i>
        #Option     "TV2FlickerFilter"   	# <i>
        #Option     "Refresh"            	# <i>
        #Option     "DisableVQ"          	# [<bool>]
        #Option     "NoDDCValue"         	# [<bool>]
        #Option     "DRIXINERAMA"        	# [<bool>]
        #Option     "DeBlockingEnable"   	# [<bool>]
        #Option     "Use2DBitBltSignature" 	# [<bool>]
        #Option     "MergedFB"           	# [<bool>]
        #Option     "NoVIAXinerama"      	# [<bool>]
        #Option     "Scrn2Pos"           	# [<str>]
        #Option     "EPIA_DVI"           	# [<bool>]
        #Option     "VideoOnDevice"      	# [<str>]
        #Option     "CCIR601"            	# [<bool>]
        #Option     "Scrn2Pos"           	# [<str>]
        #Option     "DPASetting_DVP0"    	# [<str>]
        #Option     "DPASetting_DVP1"    	# [<str>]
        #Option     "DPASetting_DFPHIGH" 	# [<str>]
        #Option     "DPASetting_DFPLOW"  	# [<str>]
        #Option     "DPASetting_VT1636"  	# [<str>]
        #Option     "DPASetting_VT1625"  	# [<str>]
        #Option     "DPASetting_VT1623"  	# [<str>]
        #Option     "DISPLAY_3DScaling"  	# [<bool>]
        #Option     "DISPLAY_3DScaling_HDMI" 	# [<bool>]
        #Option     "DISPLAY_3DScaling_LCD" 	# [<bool>]
        #Option     "VideoColorKey"      	# <i>
        #Option     "GoodPerformance"    	# [<bool>]
        #Option     "LCD_MSB_ENABLE"     	# [<bool>]
        #Option     "NoDithering"        	# [<bool>]
        #Option     "DualChannel"        	# [<bool>]
        #Option     "Interlace"          	# [<bool>]
        #Option     "FnHotkey"           	# [<str>]
        #Option     "TextureVideoSupport" 	# [<bool>]

	# dev.laptop.org #9798 Don't probe I2C busses where the camera is connected
	Option "I2CDevices" "Bus1"

	Identifier  "Card0"
	Driver      "openchrome"
	VendorName  "VIA Technologies, Inc."
	BoardName   "Unknown Board"
	BusID       "PCI:0:1:0"
EndSection

Section "Screen"
	Identifier "Screen0"
	Device     "Card0"
	Monitor    "Monitor0"
	DefaultDepth	24
	SubSection "Display"
		Viewport   0 0
		Depth     1
	EndSubSection
	SubSection "Display"
		Viewport   0 0
		Depth     4
	EndSubSection
	SubSection "Display"
		Viewport   0 0
		Depth     8
	EndSubSection
	SubSection "Display"
		Viewport   0 0
		Depth     15
	EndSubSection
	SubSection "Display"
		Viewport   0 0
		Depth 24 
		Modes "1200x900"
	EndSubSection
EndSection
EOF
