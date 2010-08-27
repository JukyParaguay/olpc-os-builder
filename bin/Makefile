CC=gcc

all: zhashfs

zhashfs: zhashfs.c
		$(CC) -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DARGTYPE=1 $^ -o $@ -I/usr/include/tomcrypt -ltomcrypt -lz

install: all
	install -D -m 0755 zhashfs $(DESTDIR)/usr/libexec/olpc-os-builder/zhashfs

clean:
	rm -f zhashfs

