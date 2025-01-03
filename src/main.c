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
    char *mv = move_to_algebric(l.moves[i]);
    wprintf(L"%s ", mv);
    free(mv);
  }
  wprintf(L"\n");
}

int main(void) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling
  bb_magic_init();

  Board *board = board_init();
  threat_board_update(board);
  char strmove[6];
  int res;
  while (true) {
    // // Player turn
    // board_info(board);
    //
    // test_print_moves(possible_move(board));
    //
    // int res = -1;
    // while (res) {
    //   scanf("%s", strmove);
    //   res = move(board, algebric_to_move(strmove, board));
    // }

    // Bot turn
    board_info(board);

    wprintf(L"BOT WHITE\n");
    Move bot = choose(board);

    char *mv = move_to_algebric(bot);
    wprintf(L"%s\n", mv);
    free(mv);

    res = move(board, bot);
    if (res) {
      wprintf(L"Bot fail, bot dumb\n");
      break;
    }

    // Bot turn
    board_info(board);

    wprintf(L"BOT BLACK\n");
    bot = choose(board);

    mv = move_to_algebric(bot);
    wprintf(L"%s\n", mv);
    free(mv);

    res = move(board, bot);
    if (res) {
      wprintf(L"Bot fail, bot dumb\n");
      break;
    }
  }

  free(board->history->list_of_move);
  free(board->history);
  board_free(board);

  return EXIT_SUCCESS;
}
