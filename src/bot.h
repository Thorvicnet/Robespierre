#ifndef BOT_H
#define BOT_H

#include "board.h"
#include "move.h"
#include "types.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

int evaluate(Board *board);
Move choose(Board *board);

#endif // BOT_H
