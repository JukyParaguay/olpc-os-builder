// Handle partitions
#include <stdio.h>
#include <stdint.h>
#include <linux/fiemap.h>
#include <linux/fs.h>

#define TFM_DESC
#include <tomcrypt.h>
#include "zlib.h"

static FILE *outfile;
static FILE *zfile;
static int hashid;
static unsigned char *zbuf;
static long zbufsize;
static long zblocksize;
static char *hashname;

static long eblocks = -1;

/*
 * A bitmap detailing which eblocks are used, and which are empty.
 * (actually a char array, one byte per eblock, because I'm lazy)
 */
static unsigned char *eblocks_used;

#define PATTERN_SIZE 4096

#define DO(x) do { run_cmd((x), __LINE__, __FILE__, #x); } while (0);
static void run_cmd(int res, int line, char *file, char *cmd)
{
   if (res != CRYPT_OK) {
      fprintf(stderr, "%s (%d)\n%s:%d:%s\n", error_to_string(res), res, file, line, cmd);
      if (res != CRYPT_NOP) {
         exit(EXIT_FAILURE);
      }
   }
}

static void write_block(long blocknum, unsigned char *buf)
{
    unsigned char md[MAXBLOCKSIZE];
    unsigned long mdlen;
    uLongf        zlen;
    int           zresult;
    int		      j;

    mdlen = sizeof(md);
    DO(hash_memory(hashid, buf, zblocksize, md, &mdlen));

    zlen = zbufsize;
    if ((zresult = compress(zbuf, &zlen, buf, zblocksize)) != Z_OK) {
        fprintf(stderr, "Compress failure at block 0x%lx - %d\n", blocknum, zresult);
    }

    fprintf(outfile, "zblock: %lx %lx %s ", blocknum, zlen, hashname);
    for(j=0; j<mdlen; j++)
        fprintf(outfile,"%02x",md[j]);
    fprintf(outfile, "\n");

    fprintf(zfile, "zblock: %lx %lx %s ", blocknum, zlen, hashname);
    for(j=0; j<mdlen; j++)
        fprintf(zfile,"%02x",md[j]);
    fprintf(zfile, "\n");
    fwrite(zbuf, sizeof(char), zlen, zfile);
    fprintf(zfile, "\n");
}

static int read_block(unsigned char *buf, FILE *infile, int is_last_block)
{
    int readlen = fread(buf, 1, zblocksize, infile);
    unsigned char *p;

    if (readlen != zblocksize && readlen && is_last_block) {
        for (p = &buf[readlen]; p < &buf[zblocksize]; p++) {
            *p = 0xff;
        }
        readlen = zblocksize;
    }
    return readlen;
}

struct fiemap *read_fiemap(int fd)
{
	struct fiemap *fiemap;
	int extents_size;

	if ((fiemap = (struct fiemap*)malloc(sizeof(struct fiemap))) == NULL) {
		fprintf(stderr, "Out of memory allocating fiemap\n");
		return NULL;
	}
	memset(fiemap, 0, sizeof(struct fiemap));

	fiemap->fm_start = 0;
	fiemap->fm_length = ~0;
	fiemap->fm_flags = 0;
	fiemap->fm_extent_count = 0;
	fiemap->fm_mapped_extents = 0;

	/* Find out how many extents there are */
	if (ioctl(fd, FS_IOC_FIEMAP, fiemap) < 0) {
		fprintf(stderr, "fiemap ioctl() failed\n");
		return NULL;
	}

	/* Read in the extents */
	extents_size = sizeof(struct fiemap_extent) *  (fiemap->fm_mapped_extents);

	/* Resize fiemap to allow us to read in the extents */
	if ((fiemap = (struct fiemap*)realloc(fiemap, sizeof(struct fiemap) +
                                         extents_size)) == NULL) {
		fprintf(stderr, "Out of memory allocating fiemap\n");
		return NULL;
	}

	memset(fiemap->fm_extents, 0, extents_size);
	fiemap->fm_extent_count = fiemap->fm_mapped_extents;
	fiemap->fm_mapped_extents = 0;

	if (ioctl(fd, FS_IOC_FIEMAP, fiemap) < 0) {
		fprintf(stderr, "fiemap ioctl() failed\n");
		return NULL;
	}

	return fiemap;
}

/* Given a file extent, determine which eblocks in the output file need to
 * represent that extent, and mark them as used in the eblocks_used map. */
static long process_extent(struct fiemap_extent *ex)
{
	long i;
	uint64_t last_byte = ex->fe_logical + ex->fe_length - 1;
	long first_eblock = ex->fe_logical / zblocksize;
	long last_eblock = last_byte / zblocksize;

	printf("Extent(%lld, %lld) occupies eblocks %d to %d\n", ex->fe_logical, ex->fe_length, first_eblock, last_eblock);
	for (i = first_eblock; i <= last_eblock; i++)
		eblocks_used[i] = 1;

	return last_eblock;
}

/*
 * Use FIEMAP to determine the extents that make up a file.
 * Allocate eblocks_used array based on length of file, and then mark
 * the set of eblocks that contain data based on the extents.
 * Returns the index of the last occupied eblock.
 */
static long read_extents(FILE *infile)
{
    int i;
    struct fiemap *fiemap = read_fiemap(fileno(infile));
    long ret = 0;

    LTC_ARGCHK(fiemap != NULL);

    eblocks_used = malloc(eblocks);
    LTC_ARGCHK(eblocks_used != 0);
    memset(eblocks_used, 0, eblocks);

	for (i=0; i < fiemap->fm_mapped_extents; i++)
		ret = process_extent(&fiemap->fm_extents[i]);

    return ret;
}

int main(int argc, char **argv)
{
    char          *fname;
    unsigned char *buf;  // EBLOCKSIZE
    FILE          *infile;
    long          i;
    off_t         insize;
    int		  readlen;

    int		  skip;

    if (argc < 6) { 
        fprintf(stderr, "%s: zblocksize hashname signed_file_name spec_file_name zdata_file_name [ #blocks ]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 7)
        eblocks = strtol(argv[6], 0, 0);

    zblocksize = strtol(argv[1], 0, 0);

    buf = malloc(zblocksize);
    LTC_ARGCHK(buf != 0);

    /*
     * For zlib compress, the destination buffer needs to be 1.001 x the
     * src buffer size plus 12 bytes
     */
    zbufsize = ((zblocksize * 102) / 100) + 12;
    zbuf = malloc(zbufsize);
    LTC_ARGCHK(zbuf != 0);

    LTC_ARGCHK(register_hash(&sha256_desc) != -1);
    LTC_ARGCHK(register_hash(&rmd160_desc) != -1);
    LTC_ARGCHK(register_hash(&md5_desc) != -1);

    hashname = argv[2];
    hashid = find_hash(hashname);
    LTC_ARGCHK(hashid >= 0);

    /* open filesystem image file */
    infile = fopen(argv[3], "rb");
    LTC_ARGCHK(infile != NULL);

    /* open output file */
    outfile = fopen(argv[4], "wb");
    LTC_ARGCHK(outfile != NULL);

	LTC_ARGCHK(fputs("[ifndef] #eblocks-written\n", outfile) >= 0);
	LTC_ARGCHK(fputs("[ifdef] last-eblock#\n", outfile) >= 0);
	LTC_ARGCHK(fputs(": pdup  ( n -- n' )  dup  last-eblock# max  ;\n", outfile) >= 0);
	LTC_ARGCHK(fputs("also nand-commands  patch pdup dup zblock:  previous\n", outfile) >= 0);
	LTC_ARGCHK(fputs("[then]\n", outfile) >= 0);
	LTC_ARGCHK(fputs("[then]\n", outfile) >= 0);

    /* open zdata file */
    zfile = fopen(argv[5], "wb");
    LTC_ARGCHK(zfile != NULL);

    if (eblocks == -1) {
        (void)fseek(infile, 0L, SEEK_END);
        insize = ftello(infile);
        (void)fseek(infile, 0L, SEEK_SET);

        eblocks = (insize + zblocksize - 1) / zblocksize;
//    LTC_ARGCHK((eblocks * zblocksize) == insize);
    }

    eblocks = read_extents(infile) + 1;

    /* Remove possible path prefix */
    fname = strrchr(argv[5], '/');
    if (fname == NULL)
        fname = argv[5];
    else
        ++fname;

    fprintf(outfile, "data: %s\n", fname);
    fprintf(outfile, "zblocks: %lx %lx\n", zblocksize, eblocks);
    fprintf(zfile,   "zblocks: %lx %lx\n", zblocksize, eblocks);

    fprintf(stdout, "Total blocks: %ld\n", eblocks);

    /* wipe the partition table first in case of partial completion */
    memset(buf, 0, zblocksize);
    write_block(0, buf);

    /* make a hash of the file */
    for (i=1; i < eblocks; i++) {
        if (!eblocks_used[i])
            continue;

        fseeko(infile, (uint64_t) i * zblocksize, SEEK_SET);
        readlen = read_block(buf, infile, i == eblocks-1);
        LTC_ARGCHK(readlen == zblocksize);

        write_block(i, buf);
        fprintf(stdout, "\r%ld", i);  fflush(stdout);
    }

    fseek(infile, 0L, SEEK_SET);
    readlen = read_block(buf, infile, 0 == eblocks-1);
    LTC_ARGCHK(readlen == zblocksize);
    write_block(0, buf);

    fprintf(outfile, "zblocks-end:\n");
    fprintf(zfile,   "zblocks-end:\n");

    fclose(infile);
    fclose(outfile);

	putchar('\n');
    return EXIT_SUCCESS;
}
