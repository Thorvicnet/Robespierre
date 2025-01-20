#ifndef BOT_H
#define BOT_H

#include "board.h"
#include "move.h"
#include "types.h"
#include "tree.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int evaluate(Board *board);
Move choose(Board *board);
Move choose2(MoveTree *tree);

#endif // BOT_H
