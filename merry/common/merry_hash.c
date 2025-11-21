#include <merry_hash.h>

msize_t merry_string_hash(mptr_t key, msize_t bucket_count) {
  mdword_t c1 = 0xcc9e2d51;
  mdword_t c2 = 0x1b873593;
  mdword_t r1 = 15;
  mdword_t r2 = 13;
  mdword_t m = 5;
  mdword_t n = 0xe6546b64;
  mdword_t hash = 0;
  msize_t len = strlen((const char *)key);

  const msdword_t nblocks = len / 4;
  const mdword_t *blocks = (const mdword_t *)(key);
  const mbptr_t tail = (const mbptr_t)((mcstr_t)key + nblocks * 4);

  mdword_t k;
  for (msdword_t i = 0; i < nblocks; i++) {
    k = blocks[i];
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;
    hash ^= k;
    hash = (hash << r2) | (hash >> (32 - r2));
    hash = hash * m + n;
  }

  k = 0;
  switch (len & 3) {
  case 3:
    k ^= tail[2] << 16;
  case 2:
    k ^= tail[1] << 8;
  case 1:
    k ^= tail[0];
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;
    hash ^= k;
  }

  hash ^= len;
  hash ^= (hash >> 16);
  hash *= 0x85ebca6b;
  hash ^= (hash >> 13);
  hash *= 0xc2b2ae35;
  hash ^= (hash >> 16);

  return hash % bucket_count;
}
