mkdir -p /home/olpc/{Activities,Library}
chown olpc:olpc /home/olpc/{Activities,Library}

for i in /build_shared/sugar-bundles/*; do
	if [ "${i:(-4)}" == ".xol" ]; then
		/usr/bin/unzip -d /home/olpc/Library -q $i
	else
		/bin/su -c "/usr/bin/sugar-install-bundle $i" olpc
	fi
done

chown -R olpc:olpc /home/olpc/{Activities,Library}
