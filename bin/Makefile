CC=gcc

all: crcimg zhashfs

crcimg: crc32.c crcimg.c
		$(CC) $^ -o $@

zhashfs: zhashfs.c
		$(CC) -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DARGTYPE=1 $^ -o $@ -I/usr/include/tomcrypt -ltomcrypt -lz

install: all
	install -D -m 0755 zhashfs $(DESTDIR)/usr/libexec/olpc-os-builder/zhashfs
	install -D -m 0755 crcimg $(DESTDIR)/usr/libexec/olpc-os-builder/crcimg

clean:
	rm -f crcimg zhashfs

