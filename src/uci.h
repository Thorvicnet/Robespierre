#ifndef UCI_H
#define UCI_H

#include "board.h"
#include "bot.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include <stdlib.h>
#include <wchar.h>

#define ENGINE_NAME "Robespierre"
#define ENGINE_AUTHOR "NonoAdadToto"
#define START_FEN                                                              \
  "rnbqkbnr / pppppppp / 8 / 8 / 8 / 8 / PPPPPPPP / RNBQKBNR w KQkq - 0 1"

char *move_to_algebric(Move move);
Move algebric_to_move(char *ch, Board *board);
int transform_board_from_fen(char *fen, Board *board);
char *create_fen_from_board(Board *board);
void uci_loop(Board *board);

#endif // UCI_H
