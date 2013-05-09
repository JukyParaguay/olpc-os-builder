# enable serial console on ttyS2
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/getty.target.wants/serial-getty@ttyS2.service

# enable serial console for runin too
mkdir -p /etc/systemd/system/runin.target.wants
ln -sf /lib/systemd/system/serial-getty@.service \
	/etc/systemd/system/runin.target.wants/serial-getty@ttyS2.service

# enable dmix and dsnoop software sound mixing (#12487, #12606)
# FIXME: this should be upstreamed (perhaps in a different form)
cat <<'EOF' > /usr/share/alsa/cards/OLPC_XO-1_75.conf
# default with dmix/dsnoop
OLPC_XO-1_75.pcm.default {
	@args [ CARD ]
	@args.CARD {
		type string
	}
	type asym
	playback.pcm {
		type plug
		slave.pcm {
			@func concat
			strings [ "dmix:CARD=" $CARD ]
		}
	}
	capture.pcm {
		type plug
		slave.pcm {
			@func concat
			strings [ "dsnoop:CARD=" $CARD ]
		}
	}
}
EOF
