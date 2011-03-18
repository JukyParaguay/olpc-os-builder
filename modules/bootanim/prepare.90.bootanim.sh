# Copyright (C) 2011 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib

imgdir=$(read_config bootanim imgdir)
cache=$cachedir/bootanim
cacheframes=$cachedir/bootanim-frames

STATIC_IMAGES="frame00.png ul_warning.png"
FRAME_IMAGES="frame00.png frame01.png frame02.png frame03.png
	frame04.png frame05.png frame06.png frame07.png frame08.png frame09.png
	frame10.png frame11.png frame12.png frame13.png frame14.png frame15.png
	frame16.png frame17.png frame18.png frame19.png frame20.png frame21.png
	frame22.png frame23.png frame24.png frame25.png ul_warning.png"

if [ -n "$imgdir" -a -e "$imgdir" ]; then
	if [ ! -x '/usr/bin/ppmto565.py' -o ! -x '/usr/bin/calcdelta' ]; then
		echo Please install olpc-bootanim-tools >&2
		exit 1
	fi
	if [ ! -x '/usr/bin/pngtopnm' ]; then
		echo Please install netpbm-progs >&2
		exit 1
	fi
	mkdir -p "$cache"
	mkdir -p "$cacheframes"
	for img in $STATIC_IMAGES; do
		src=$imgdir/$img
		target=$cache/${img%.*}.565

		# like make
		if [ ! -e "$target" -o "$src" -nt "$target" ];then
			echo "Processing $src"
			pngtopnm "$src" | ppmto565.py -o "$target.tmp"
			mv "$target.tmp" "$target"
		fi
	done

	rebuilddelta=0
	for img in $FRAME_IMAGES; do
		src=$imgdir/$img
		target=$cacheframes/${img%.*}.565

		# like make
		if [ ! -e "$target" -o "$src" -nt "$target" ];then
			echo "Processing $src"
			pngtopnm "$src" | ppmto565.py -o "$target.tmp"
			mv "$target.tmp" "$target"
			rebuilddelta=1
		fi
	done
	if [ "$rebuilddelta" == 1 -o ! -e "$cache/deltas" ];then
		echo "Creating delta sequence"
		# unfortunately,
		tmpdir="/tmp/oob_bootanim.$$"
		mkdir -p "$tmpdir"
		pushd "$tmpdir"
		for img in $FRAME_IMAGES; do
			echo $cacheframes/${img%.*}.565 >> frames
		done
		# calcdelta reads a 'frames' file listing
		# the files to process
		echo calcdelta frames
		calcdelta frames
		mv deltas $cache/
		popd
		rm -fr "$tmpdir"
	fi
fi
