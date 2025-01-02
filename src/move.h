#ifndef MOVE_H
#define MOVE_H

#include "board.h"
#include "threat.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

bool move_check_validity(Board *board, int orig[2], int dest[2]);
int move(Board *board, Move move);
MoveList possible_move(Board *board);

#endif // MOVE_H
