#include "bb.h"
#include "board.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include "uci.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int main(void) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling
  bb_magic_init();

  Board *board = board_init();
  threat_board_update(board);
  int orig[2], dest[2];
  char strmove[6];
  while (true) {
    board_info(board);
    board_bb_info(board);
    //scanf("%d %d, %d %d", &orig[0], &orig[1], &dest[0], &dest[1]);
    scanf("%s", strmove);
    move(board, (Move){board_get(board, orig[0] + orig[1] * 8),
                       {orig[0], orig[1]},
                       {dest[0], dest[1]}});
    //Move mo = algebric_to_move(strmove, board);
    move(board, mo);
  }

  return EXIT_SUCCESS;
}
