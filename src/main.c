#include "bb.h"
#include "board.h"
#include "bot.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include "uci.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

void test_print_moves(MoveList l) {
  for (int i = 0; i < l.count; i++) {
    wprintf(L"%s ", move_to_algebric(l.moves[i]));
  }
  wprintf(L"\n");
}

int main(void) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling
  bb_magic_init();

  Board *board = board_init();
  threat_board_update(board);
  char strmove[6];
  while (true) {
    // Player turn
    board_info(board);

    wprintf(L"%d\n", possible_move(board).count);
    test_print_moves(possible_move(board));

    int res = -1;
    while (res) {
      scanf("%s", strmove);
      res = move(board, algebric_to_move(strmove, board));
    }

    // Bot turn
    board_info(board);

    wprintf(L"BOT\n");
    Move bot = choose(board);
    wprintf(L"%s\n", move_to_algebric(bot));
    res = move(board, bot);
    wprintf(L"Bot fail, bot dumb\n");
  }

  return EXIT_SUCCESS;
}
