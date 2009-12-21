# normalize mtime field to reduce changes in this file during updates (#4259)
sed -i -e 's/mtime="[^"]*"/mtime="0"/g' \
	/etc/gconf/gconf.xml.defaults/%gconf-tree.xml

