#ifndef BOT_H
#define BOT_H

#include "move.h"
#include "board.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

int evaluate (Board *board);
//Move choose (Board *board, int color);

#endif // BOT_H
