/*
 * Here follows an extended implementation of the `SuperHashFunction`,
 * originally found at <https://www.azillionmonkeys.com/qed/hash.html>,
 * licensed as <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt>
 */

#ifndef LF_SUPERHASH_H
#define LF_SUPERHASH_H

#include <stdint.h>

#define to32bits(d) *((const uint32_t *)d)

uint64_t superhash(const char *data, int len);

#endif // LF_SPERHASH_H
