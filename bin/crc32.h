/* Ripped from the linux kernel crc32 code
 * 
 * This source code is licensed under the GNU General Public License,
 * Version 2.  
 */
#include <stdint.h>
#include <stddef.h>

typedef uint32_t u32;

extern u32  crc32_le(u32 crc, unsigned char const *p, size_t len);
#define crc32(seed, data, length)  crc32_le(seed, (unsigned char const *)data, length)
#define CRCPOLY_LE 0xedb88320

