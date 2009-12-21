// Handle partitions
#include <stdio.h>

#define TFM_DESC
#include <tomcrypt.h>
#include "zlib.h"

#define DO(x) do { run_cmd((x), __LINE__, __FILE__, #x); } while (0);
void run_cmd(int res, int line, char *file, char *cmd)
{
   if (res != CRYPT_OK) {
      fprintf(stderr, "%s (%d)\n%s:%d:%s\n", error_to_string(res), res, file, line, cmd);
      if (res != CRYPT_NOP) {
         exit(EXIT_FAILURE);
      }
   }
}

int main(int argc, char **argv)
{
    char          *fname;
    char          *hashname;
    unsigned char *buf;  // EBLOCKSIZE
    unsigned char md[MAXBLOCKSIZE], sig[512];
    unsigned long mdlen;
    FILE          *infile, *outfile;
    long          eblocks = -1;
    long          i;
    long	  zblocksize, zbufsize;
    off_t         insize;
    int		  hashid, readlen;
    int		  j;

    int		  allf;
    int           zresult;
    FILE          *zfile;
    uLongf        zlen;
    unsigned char *p;
    unsigned char *zbuf; // ZBUFSIZE

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
    fprintf(outfile, "zblocks: %x %x\n", zblocksize, eblocks);
    fprintf(zfile,   "zblocks: %x %x\n", zblocksize, eblocks);

    fprintf(stdout, "Total blocks: %d\n", eblocks);

    /* make a hash of the file */
    for (i=0; i < eblocks; i++) {
        readlen = fread(buf, 1, zblocksize, infile);
        if (readlen != zblocksize && readlen && i == eblocks-1) {
            for (p = &buf[readlen]; p < &buf[zblocksize]; p++) {
                *p = 0xff;
            }
            readlen = zblocksize;
        }            
        LTC_ARGCHK(readlen == zblocksize);

#ifdef notdef
        allf = 1;
        for (p = (unsigned char *)buf; p < &buf[zblocksize]; p++) {
            if (*p != 0xff) {
                allf = 0;
                break;
            }
        }
#else
        allf = 0;
#endif

        if (!allf) {
            mdlen = sizeof(md);
            DO(hash_memory(hashid, buf, zblocksize, md, &mdlen));

            zlen = zbufsize;
            if ((zresult = compress(zbuf, &zlen, buf, zblocksize)) != Z_OK) {
                fprintf(stderr, "Compress failure at block 0x%x - %d\n", i, zresult);
            }

            fprintf(outfile, "zblock: %x %x %s ", i, zlen, hashname);
            for(j=0; j<mdlen; j++)
                fprintf(outfile,"%02x",md[j]);
            fprintf(outfile, "\n");

            fprintf(zfile, "zblock: %x %x %s ", i, zlen, hashname);
            for(j=0; j<mdlen; j++)
                fprintf(zfile,"%02x",md[j]);
            fprintf(zfile, "\n");
            fwrite(zbuf, sizeof(char), zlen, zfile);
            fprintf(zfile, "\n");
        }
        fprintf(stdout, "\r%d", i);  fflush(stdout);
    }
    fprintf(outfile, "zblocks-end:\n");
    fprintf(zfile,   "zblocks-end:\n");

    fclose(infile);
    fclose(outfile);

	putchar('\n');
    return EXIT_SUCCESS;
}
