#ifndef BOT_H
#define BOT_H

#include "board.h"
#include "move.h"
#include "tree.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int evaluate(Board *board);
Move choose(MoveTree *tree, Board *board);

#endif // BOT_H
