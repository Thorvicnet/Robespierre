#ifndef TYPES_H
#define TYPES_H

#define PIECE(x) (x & 0x0F)
#define COLOR(x) (x & 0xF0)

#define SET_BIT(bb, square) ((bb) |= (1ULL << (square)))
#define CLEAR_BIT(bb, square) ((bb) &= ~(1ULL << (square)))
#define IS_BIT_SET(bb, square) ((bb) & (1ULL << (square)))

#include <stdbool.h>

typedef unsigned long long Bb;

typedef struct {
  unsigned char piece;
  unsigned char from;
  unsigned char to;
  unsigned char promote;
} Move;

typedef struct {
  int castle;
  int capture;
  Bb ep;
} Undo;

typedef struct {
  int last_move;
  Move *list_of_move;
} Stack;

typedef struct t {
  bool children_filled;
  Move *moves;
  int moves_count;
  struct t **children;
} MoveTree;


typedef struct {
  int squares[64];
  int color;
  Stack *history;

  int castle;
  Bb ep;

  Bb all;
  Bb white;
  Bb black;
  Bb white_pawns;
  Bb black_pawns;
  Bb white_knights;
  Bb black_knights;
  Bb white_bishops;
  Bb black_bishops;
  Bb white_rooks;
  Bb black_rooks;
  Bb white_queens;
  Bb black_queens;
  Bb white_kings;
  Bb black_kings;

  Bb white_threat;
  Bb black_threat;
} Board;

#endif // TYPES_H
