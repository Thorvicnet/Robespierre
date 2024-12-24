#ifndef THREAT_H
#define THREAT_H

#include "board.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

bool check_threatened(Board *board, int cell[2], int color, int depth);
void threat_board_update(Board *board);

#endif // THREAT_H
