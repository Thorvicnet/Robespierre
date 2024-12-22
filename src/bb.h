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

void bb_board_init(Board *board);
void bb_print(Bb b);

#endif // BB_H
