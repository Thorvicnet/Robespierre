#ifndef TYPES_H
#define TYPES_H

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
} Board;

#endif // TYPES_H
