#ifndef MOVE_H
#define MOVE_H

#include "board.h"
#include "threat.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_MOVES 256

bool move_check_validity(Board *board, int from, int to);
int move_make(Board *board, Move *move, Undo *undo);
int move_undo(Board *board, Move *move, Undo *undo);
int move_possible(Board *board, Move *moves);
void move_list_free(MoveList *list);

#endif // MOVE_H
