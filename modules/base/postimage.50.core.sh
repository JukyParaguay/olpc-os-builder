# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

. $OOB__shlib
buildnr=$(read_buildnr)
treetar=$outputdir/$(image_name).tree.tar.lzma
pkglist=$outputdir/$(image_name).packages.txt
actlist=$outputdir/$(image_name).activities.txt
liblist=$outputdir/$(image_name).libraries.txt
fillist=$outputdir/$(image_name).files.txt

maketree=$(read_config base make_tree_tarball)
if [[ "$maketree" == "1" ]]; then
	echo "Make tree tarball..."
	tar -c -C $fsmount . | lzma -1 > $treetar

	echo "Checksum tree tarball..."
	md5sum $treetar > $treetar.md5
fi

versioned_fs=$(read_config base versioned_fs)
if [ "$versioned_fs" = "1" ]; then
	chroot_path=$fsmount/versions/pristine/$buildnr
else
	chroot_path=$fsmount
fi

chroot $chroot_path /bin/rpm -qa | sort > $pkglist

# generate an activity version listing for comparison.
find $fsmount -name activity.info \
    -exec awk '/activity_version/ { print FILENAME "-" $3; }' {} \; | \
  sed -e "s%$fsmount%%g" \
      -e 's/\/home\/olpc\/Activities\///g' \
      -e 's/.activity\/activity\/activity.info//g' | \
  sort > $actlist

# generate a library version listing for comparison.
find $fsmount -name library.info \
    -exec awk '/library_version/ { print FILENAME "-" $3; }' {} \; | \
  sed -e "s%$fsmount%%g" \
      -e 's/\/home\/olpc\/Library\///g' \
      -e 's/\/library\/library.info//g' | \
  sort > $liblist

# generate a file listing for comparison,
# removing the build number.
find $fsmount | \
  sed -e "s%$fsmount%%g" \
      -e "s%/versions/pristine/${buildnr}%/versions/pristine/\${BUILD}%g" \
      -e "s%/versions/run/${buildnr}%/versions/run/\${BUILD}%g" \
      -e "s%/versions/contents/${buildnr}%/versions/contents/\${BUILD}%g" | \
  gzip - > $fillist.gz
