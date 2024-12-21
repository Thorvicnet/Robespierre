#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <stdlib.h>
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



typedef struct {
  int last_move;
  int* list_of_move;
} Stack;



typedef struct {
  int squares[64];
  int moves;
  int color;
  Stack* historic;
} Board;

Board *board_init(void);
void board_empty(Board *board);
void board_print(Board *board);
void board_info(Board *board);
void board_set(Board *board, int sq, int piece);
int board_get(Board *board, int sq);
int get_last_move(Board *board);
void board_free(Board *board);

#endif // BOARD_H
