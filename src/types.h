#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
typedef struct {
  int piece;
  int orig[2];
  int dest[2];
  bool capture;
} Move;

typedef struct {
  Move* list;
  int nb;
} List_of_move ;

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
