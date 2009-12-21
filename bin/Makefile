CC=gcc

all: crcimg zhashfs

crcimg: crc32.c crcimg.c
		$(CC) $^ -o $@

zhashfs: zhashfs.c
		$(CC) -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DARGTYPE=1 $^ -o $@ -I/usr/include/tomcrypt -ltomcrypt -lz

clean:
	rm -f crcimg zhashfs

