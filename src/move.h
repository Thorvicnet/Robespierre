#ifndef MOVE_H
#define MOVE_H

#include "board.h"
#include "threat.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

bool move_check_validity(Board *board, int from, int to);
int move(Board *board, Move move);
MoveList move_possible(Board *board);
void move_list_free(MoveList *list);

#endif // MOVE_H
