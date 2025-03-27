#include "./superhash.h"

uint64_t superhash(const char *data, int len) {
  if (!data || len <= 0) {
    return 0;
  }

  uint64_t hash = len, tmp;
  int rem = len & 3;
  len >>= 2;

  for (; len > 0; len--) {
    hash += to32bits(data);
    tmp = (to32bits(data + 4) << 11) ^ hash;
    hash = (hash << 16) ^ tmp;
    // Move 4 bytes forward since we're reading 32 bits.
    data += 4 * sizeof(uint8_t);
    hash += hash >> 11;
  }

  switch (rem) {
  case 3:
    hash += to32bits(data);
    hash ^= hash << 16;
    hash ^= ((signed char)data[sizeof(uint32_t)]) << 18;
    hash += hash >> 11;
    break;
  case 2:
    hash += to32bits(data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1:
    hash += (signed char)*data;
    hash ^= hash << 10;
    hash += hash >> 1;
    break;
  }

  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
}
