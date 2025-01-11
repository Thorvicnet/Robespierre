#ifndef BOARD_H
#define BOARD_H

#include "bb.h"
#include "history.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define WHITE 0x00
#define BLACK 0x10

#define EMPTY 0x00
#define PAWN 0x01
#define KNIGHT 0x02
#define BISHOP 0x03
#define ROOK 0x04
#define QUEEN 0x05
#define KING 0x06

#define WHITE_PAWN 0x01
#define WHITE_KNIGHT 0x02
#define WHITE_BISHOP 0x03
#define WHITE_ROOK 0x04
#define WHITE_QUEEN 0x05
#define WHITE_KING 0x06

#define BLACK_PAWN 0x11
#define BLACK_KNIGHT 0x12
#define BLACK_BISHOP 0x13
#define BLACK_ROOK 0x14
#define BLACK_QUEEN 0x15
#define BLACK_KING 0x16

#define WHITE_CASTLE 3
#define BLACK_CASTLE 12

#define WHITE_CASTLE_KINGSIDE 1
#define WHITE_CASTLE_QUEENSIDE 2
#define BLACK_CASTLE_KINGSIDE 4
#define BLACK_CASTLE_QUEENSIDE 8

Board *board_init(void);
void board_empty(Board *board);
void board_print(Board *board);
void board_info(Board *board);
void board_set(Board *board, int sq, int piece);
int board_get(Board *board, int sq);
Move board_last_move(Board *board);
void board_add_move(Board *board, Move move);
void board_list_moves(Board *board);
void board_free(Board *board);
Board *board_copy(Board *board);
void board_sync_bb(Board *board);
void board_bb_info(Board *board);

#endif // BOARD_H
