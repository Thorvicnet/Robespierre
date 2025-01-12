#ifndef UCI_H
#define UCI_H

#include "board.h"
#include "types.h"
#include <stdlib.h>
#include <wchar.h>

char *move_to_algebric(Move move);
Move algebric_to_move(char *ch, Board *board);
int transform_board_from_fen(char* fen, Board *board);
char* create_fen_from_board(Board *board);
#endif // UCI_H
