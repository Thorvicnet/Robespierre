#ifndef TYPES_H
#define TYPES_H

#define PIECE(x) ((x) & 0x0F)
#define COLOR(x) ((x) & 0xF0)

typedef unsigned long long bb;

typedef struct {
  int piece;
  int orig[2];
  int dest[2];
} Move;

typedef struct {
  int last_move;
  Move *list_of_move;
} Stack;

typedef struct {
  int squares[64];
  int color;
  Stack *history;
  bb all;
  bb white;
  bb black;
  bb white_pawns;
  bb black_pawns;
  bb white_knights;
  bb black_knights;
  bb white_bishops;
  bb black_bishops;
  bb white_rooks;
  bb black_rooks;
  bb white_queens;
  bb black_queens;
  bb white_kings;
  bb black_kings;
} Board;

#endif // TYPES_H
