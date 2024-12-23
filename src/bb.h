#ifndef BB_H
#define BB_H

#include "types.h"
#include <wchar.h>

extern const Bb ROOK_MASKS[64];
extern const Bb BISHOP_MASKS[64];
extern const Bb KNIGHT_MASKS[64];
extern const Bb KING_MASKS[64];
extern const Bb PAWN_ATTACK_MASKS_WHITE[64];
extern const Bb PAWN_ATTACK_MASKS_BLACK[64];

extern const Bb ROOK_MAGIC[64];
extern const Bb BISHOP_MAGIC[64];

extern Bb ROOK_ATTACKS[64][4096];
extern Bb BISHOP_ATTACKS[64][4096];

void bb_board_init(Board *board);
void bb_print(Bb b);
Bb bb_rook_attacks(Bb occ, int sq);
Bb bb_bishop_attacks(Bb occ, int sq);
void bb_magic_init(void);

#endif // BB_H
