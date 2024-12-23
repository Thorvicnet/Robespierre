// From https://www.chessprogramming.org/Looking_for_Magics by Tord Romstad
// Computes magics for rooks and bishops

#include "../src/types.h"
#include <stdio.h>
#include <stdlib.h>

Bb random_Bb() {
  Bb u1, u2, u3, u4;
  u1 = (Bb)(random()) & 0xFFFF;
  u2 = (Bb)(random()) & 0xFFFF;
  u3 = (Bb)(random()) & 0xFFFF;
  u4 = (Bb)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

Bb random_Bb_fewbits() { // Too have better candidates
  return random_Bb() & random_Bb() & random_Bb();
}

int count_1s(Bb b) {
  int r;
  for (r = 0; b; r++, b &= b - 1)
    ;
  return r;
}

const int BitTable[64] = {63, 30, 3,  32, 25, 41, 22, 33, 15, 50, 42, 13, 11,
                          53, 19, 34, 61, 29, 2,  51, 21, 43, 45, 10, 18, 47,
                          1,  54, 9,  57, 0,  35, 62, 31, 40, 4,  49, 5,  52,
                          26, 60, 6,  23, 44, 46, 27, 56, 16, 7,  39, 48, 24,
                          59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

int pop_1st_bit(Bb *bb) {
  Bb b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

Bb index_to_Bb(int index, int bits, Bb m) {
  int i, j;
  Bb result = 0ULL;
  for (i = 0; i < bits; i++) {
    j = pop_1st_bit(&m);
    if (index & (1 << i))
      result |= (1ULL << j);
  }
  return result;
}

const Bb ROOK_MASKS[64] = {
    0x101010101017e,    0x202020202027c,    0x404040404047a,
    0x8080808080876,    0x1010101010106e,   0x2020202020205e,
    0x4040404040403e,   0x8080808080807e,   0x1010101017e00,
    0x2020202027c00,    0x4040404047a00,    0x8080808087600,
    0x10101010106e00,   0x20202020205e00,   0x40404040403e00,
    0x80808080807e00,   0x10101017e0100,    0x20202027c0200,
    0x40404047a0400,    0x8080808760800,    0x101010106e1000,
    0x202020205e2000,   0x404040403e4000,   0x808080807e8000,
    0x101017e010100,    0x202027c020200,    0x404047a040400,
    0x8080876080800,    0x1010106e101000,   0x2020205e202000,
    0x4040403e404000,   0x8080807e808000,   0x1017e01010100,
    0x2027c02020200,    0x4047a04040400,    0x8087608080800,
    0x10106e10101000,   0x20205e20202000,   0x40403e40404000,
    0x80807e80808000,   0x17e0101010100,    0x27c0202020200,
    0x47a0404040400,    0x8760808080800,    0x106e1010101000,
    0x205e2020202000,   0x403e4040404000,   0x807e8080808000,
    0x7e010101010100,   0x7c020202020200,   0x7a040404040400,
    0x76080808080800,   0x6e101010101000,   0x5e202020202000,
    0x3e404040404000,   0x7e808080808000,   0x7e01010101010100,
    0x7c02020202020200, 0x7a04040404040400, 0x7608080808080800,
    0x6e10101010101000, 0x5e20202020202000, 0x3e40404040404000,
    0x7e80808080808000};

const Bb BISHOP_MASKS[64] = {
    0x40201008040200, 0x402010080400,   0x4020100a00,     0x40221400,
    0x2442800,        0x204085000,      0x20408102000,    0x2040810204000,
    0x20100804020000, 0x40201008040000, 0x4020100a0000,   0x4022140000,
    0x244280000,      0x20408500000,    0x2040810200000,  0x4081020400000,
    0x10080402000200, 0x20100804000400, 0x4020100a000a00, 0x402214001400,
    0x24428002800,    0x2040850005000,  0x4081020002000,  0x8102040004000,
    0x8040200020400,  0x10080400040800, 0x20100a000a1000, 0x40221400142200,
    0x2442800284400,  0x4085000500800,  0x8102000201000,  0x10204000402000,
    0x4020002040800,  0x8040004081000,  0x100a000a102000, 0x22140014224000,
    0x44280028440200, 0x8500050080400,  0x10200020100800, 0x20400040201000,
    0x2000204081000,  0x4000408102000,  0xa000a10204000,  0x14001422400000,
    0x28002844020000, 0x50005008040200, 0x20002010080400, 0x40004020100800,
    0x20408102000,    0x40810204000,    0xa1020400000,    0x142240000000,
    0x284402000000,   0x500804020000,   0x201008040200,   0x402010080400,
    0x2040810204000,  0x4081020400000,  0xa102040000000,  0x14224000000000,
    0x28440200000000, 0x50080402000000, 0x20100804020000, 0x40201008040200};

Bb ratt(int sq, Bb block) { // rook attacks
  Bb result = 0ULL;
  int rk = sq / 8, fl = sq % 8, r, f;
  for (r = rk + 1; r <= 7; r++) {
    result |= (1ULL << (fl + r * 8));
    if (block & (1ULL << (fl + r * 8)))
      break;
  }
  for (r = rk - 1; r >= 0; r--) {
    result |= (1ULL << (fl + r * 8));
    if (block & (1ULL << (fl + r * 8)))
      break;
  }
  for (f = fl + 1; f <= 7; f++) {
    result |= (1ULL << (f + rk * 8));
    if (block & (1ULL << (f + rk * 8)))
      break;
  }
  for (f = fl - 1; f >= 0; f--) {
    result |= (1ULL << (f + rk * 8));
    if (block & (1ULL << (f + rk * 8)))
      break;
  }
  return result;
}

Bb batt(int sq, Bb block) { // bishop attacks
  Bb result = 0ULL;
  int rk = sq / 8, fl = sq % 8, r, f;
  for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
    result |= (1ULL << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
    result |= (1ULL << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
    result |= (1ULL << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
    result |= (1ULL << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  return result;
}

int transform(Bb b, Bb magic, int bits) {
  return (int)((b * magic) >> (64 - bits)); // 64 - bits is the shift
}

Bb find_magic(int sq, int m, int bishop) {
  Bb mask, b[4096], a[4096], used[4096], magic;
  int i, j, k, n, fail;

  mask = bishop ? BISHOP_MASKS[sq] : ROOK_MASKS[sq];
  n = count_1s(mask);

  for (i = 0; i < (1 << n); i++) {
    b[i] = index_to_Bb(i, n, mask);
    a[i] = bishop ? batt(sq, b[i]) : ratt(sq, b[i]);
  }
  for (k = 0; k < 100000000; k++) {
    magic = random_Bb_fewbits();
    if (count_1s((mask * magic) & 0xFF00000000000000ULL) < 6)
      continue;
    for (i = 0; i < 4096; i++)
      used[i] = 0ULL;
    for (i = 0, fail = 0; !fail && i < (1 << n); i++) {
      j = transform(b[i], magic, m);
      if (used[j] == 0ULL)
        used[j] = a[i];
      else if (used[j] != a[i])
        fail = 1;
    }
    if (!fail)
      return magic;
  }
  printf("***Failed***\n");
  return 0ULL;
}

int RBits[64] = {12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10,
                 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10,
                 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10,
                 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
                 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

int BBits[64] = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5,
                 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
                 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5,
                 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

int main() {
  int square;

  printf("const Bb ROOK_MAGIC[64] = {\n");
  for (square = 0; square < 64; square++)
    printf("  0x%llxULL,\n", find_magic(square, RBits[square], 0));
  printf("};\n\n");

  printf("const Bb BISHOP_MAGIC[64] = {\n");
  for (square = 0; square < 64; square++)
    printf("  0x%llxULL,\n", find_magic(square, BBits[square], 1));
  printf("};\n\n");

  return 0;
}
