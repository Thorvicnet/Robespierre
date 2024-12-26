#ifndef UCI_H
#define UCI_H
#include "types.h"

char* move_to_algebric(Move move);
Move algebric_to_move(char* ch, Board *board);

#endif 
