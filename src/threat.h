#ifndef THREAT_H
#define THREAT_H

#include "bb.h"
#include "board.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

void threat_board_update(Board *board);
bool threat_check(Board *board);

#endif // THREAT_H
