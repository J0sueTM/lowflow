/*
 * MurmurHash3 - A fast and efficient non-cryptographic hash function.
 * Copyright (c) 2011, Austin Appleby
 *
 * Licensed under the MIT License:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "./murmurhash3.h"

uint32_t murmurhash3(const char *data, size_t size)
{
    uint32_t seed =
        0x9747b28c; // You can change the seed if you like
    uint32_t hash = seed;
    size_t i = 0;

    while (i + 4 <= size) {
        uint32_t k = * (uint32_t *)(data + i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);     // Rotate left by 15
        k *= 0x1b873593;

        hash ^= k;
        hash = (hash << 13) | (hash >> 19);     // Rotate left by 13
        hash = hash * 5 + 0xe6546b64;

        i += 4;
    }

    // Handle remaining bytes (less than 4 bytes)
    uint32_t k = 0;
    size_t remaining = size - i;
    if (remaining) {
        for (size_t j = 0; j < remaining; ++j) {
            k |= (unsigned char) data[i + j] << (8 * j);
        }
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        hash ^= k;
    }

    // Finalization
    hash ^= size;
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;

    return hash;
}
