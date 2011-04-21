// Handle partitions
#include <stdio.h>

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

int main(int argc, char **argv)
{
    char          *fname;
    unsigned char *buf;  // EBLOCKSIZE
    FILE          *infile;
    long          eblocks = -1;
    long          i;
    off_t         insize;
    int		  readlen;

    int		  skip;

    unsigned char *pbuf;        // fill pattern buffer
    char          *pname;       // fill pattern file name
    FILE          *pfile;       // fill pattern file
    int           patterned, n;

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

    /* open and read an optional fill pattern file */
    pbuf = NULL;
    pname = strcat(argv[3], ".fill");
    pfile = fopen(pname, "rb");
    if (pfile != NULL) {
        pbuf = malloc(PATTERN_SIZE);
        LTC_ARGCHK(pbuf != NULL);
        n = fread(pbuf, 1, PATTERN_SIZE, pfile);
        LTC_ARGCHK(n == PATTERN_SIZE);
        fclose(pfile);
    }

    /* open output file */
    outfile = fopen(argv[4], "wb");
    LTC_ARGCHK(outfile != NULL);

	LTC_ARGCHK(fputs("[ifndef] #eblocks-written\n", outfile) >= 0);
	LTC_ARGCHK(fputs(": pdup  ( n -- n' )  dup  last-eblock# max  ;\n", outfile) >= 0);
	LTC_ARGCHK(fputs("also nand-commands  patch pdup dup zblock:  previous\n", outfile) >= 0);
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

    fseek(infile, zblocksize, SEEK_SET);

    /* make a hash of the file */
    for (i=1; i < eblocks; i++) {
        readlen = read_block(buf, infile, i == eblocks-1);
        LTC_ARGCHK(readlen == zblocksize);

#ifdef notdef
        skip = 1;
        for (p = (unsigned char *)buf; p < &buf[zblocksize]; p++) {
            if (*p != 0xff) {
                skip = 0;
                break;
            }
        }
#else
        skip = 0;
#endif

        if (pbuf) {
            /* check if this zblock is fully patterned as unused, and if
            any parts of the zblock are patterned then zero them, for ease
            of compression */

            patterned = 1;
            for (n = 0; n < (zblocksize / PATTERN_SIZE); n++) {
                if (memcmp(&buf[n*PATTERN_SIZE], pbuf, PATTERN_SIZE)) {
                    patterned = 0;
                } else {
                    memset(&buf[n*PATTERN_SIZE], 0, PATTERN_SIZE);
                }
            }

            /* skip any block that is fully patterned, thus relying on the
            fs-update card erase-blocks */

            if (patterned) skip++;
        }

        if (!skip)
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
