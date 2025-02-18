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

#define MAX_DEPTH 64

int evaluate(Board *board);
int iterative_deepening(Board *board, Move *best_move, double max_allowed);

#endif // BOT_H
