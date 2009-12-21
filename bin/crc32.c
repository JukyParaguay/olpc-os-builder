/* Ripped from the linux kernel crc32 code
 * 
 * This source code is licensed under the GNU General Public License,
 * Version 2.  
 */

#include "crc32.h"

u32 crc32_le(u32 crc, unsigned char const *p, size_t len)
{
	int i;
	while (len--) {
		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
	}
	return crc;
}

