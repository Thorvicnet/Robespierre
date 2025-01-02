#ifndef UCI_H
#define UCI_H

#include "board.h"
#include "types.h"
#include <stdlib.h>
#include <wchar.h>

char *move_to_algebric(Move move);
Move algebric_to_move(char *ch, Board *board);

#endif // UCI_H
