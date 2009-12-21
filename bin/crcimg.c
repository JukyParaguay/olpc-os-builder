/*
 * crcimg - calculates a CRC32 for each 0x20000 block of the input file
 * Used for checking JFFS2 NAND FLASH installation images.
 * Copyright 2007, Mitch Bradley
 * License: GPL v2
 * Tip 'o the hat to Richard Smith
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "crc32.h"

#define VER_MAJOR   0
#define VER_MINOR   1
#define VER_RELEASE 0

#define EBSIZE 0x20000

static unsigned char buf[EBSIZE];
static FILE *imagefile;
static FILE *crcfile;
char outfilename[FILENAME_MAX];

void usage(const char *name)
{
	printf("%s Ver: %d.%d.%d\n",name,VER_MAJOR,VER_MINOR,VER_RELEASE);
	printf("usage: %s [file.img]\n", name);
	printf("Creates file.crc containing CRCs for each 0x20000 byte block in file.img\n");
	printf("With no arguments, filters standard input, writing CRC list to standard output\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int opt=0;
	int option_index = 0;
	size_t bytes_read=0;

	static struct option long_options[]= {
		{ "help", 		0, 0, 'h' },
		{ 0, 0, 0, 0 }
	};
	
	setbuf(stdout, NULL);
	while ((opt = getopt_long(argc, argv, "h", long_options,
					&option_index)) != EOF) {
		switch (opt) {
			case 'h':
			default:
				usage(argv[0]);
				break;
		}
	}

	char *filename = NULL;

	if (optind < argc) {
		/* Filename supplied */
		filename = argv[optind++];
		if ((strlen(filename) >= 4) && (strcmp(filename + strlen(filename) - 4, ".img") == 0)) {
			strncpy(outfilename, filename, FILENAME_MAX);
			strcpy(outfilename + strlen(filename) - 4, ".crc");
		} else {
			strncpy(outfilename, filename, FILENAME_MAX);
			strncat(outfilename, ".crc", FILENAME_MAX - strlen(outfilename));
		}
		if ((imagefile = fopen(filename, "r+")) == NULL) {
			perror(filename);
			exit(1);
		}
		if ((crcfile = fopen(outfilename, "w+")) == NULL) {
			perror(outfilename);
			exit(1);
		}
	} else {
		/* Use standard in/out */
		imagefile = stdin;
		crcfile = stdout;
	}

	while ((bytes_read = fread(buf, sizeof(char), EBSIZE, imagefile)) == EBSIZE) {
		fprintf(crcfile, "%08lx\n", (unsigned long)~crc32(0xffffffff, buf, EBSIZE));
	}

	if (bytes_read != 0) {
		printf("Input file size is not a multiple of 0x%x - residue 0x%x\n", EBSIZE, bytes_read);
		fclose(crcfile);
		exit(1);
	}

	fclose(crcfile);
	return 0;
}
