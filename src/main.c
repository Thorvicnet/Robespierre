#include "bb.h"
#include "board.h"
#include "bot.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include "uci.h"
#include "tree.h"
#include <locale.h>
#include <stdbool.h>
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
  MoveTree* tree = create_tree(board);
  char strmove[6];
  int res;
  while (true) {
    // Bot turn
    if (1 == 1){
      wprintf(L"BOT WHITE\n");
      board_info(board);

      test_print_moves(move_possible(board));
      
      //Move bot = choose(board);
      Move bot = choose2(tree);

      char *mv = move_to_algebric(bot);
      wprintf(L"%s\n", mv);
      free(mv);

      res = move(board, bot);
      if (res) {
        wprintf(L"Bot fail, bot dumb\n");
        break;
      }

      tree = partially_free_tree(tree);
    }
    

    // Bot turn
    if (0 == 1){
      wprintf(L"BOT BLACK\n");
      board_info(board);

      test_print_moves(move_possible(board));

      //Move bot = choose(board);
      Move bot = choose2(tree);

      char *mv = move_to_algebric(bot);
      wprintf(L"%s\n", mv);
      free(mv);

      res = move(board, bot);
      if (res) {
        wprintf(L"Bot fail, bot dumb\n");
        break;
      }

      tree = partially_free_tree(tree);
    }
    
    // Player turn
    if (1 == 1){
      board_info(board);
      int res = -1;
      Move m;
      while (res) {
        scanf("%s", strmove);
        m = algebric_to_move(strmove, board);
        if (move_check_validity(board, m.orig, m.dest)) {
          res = move(board, m);
        }
      }
      wprintf(L"test0\n");
      int k = search_move_in_tree(tree, m);
      wprintf(L"%s\n", move_to_algebric(tree->moves->moves[k]));
      board_info(tree -> children[k] -> board);
      tree_swap(tree, k);
      tree = partially_free_tree(tree);
    }
  }

  free(board->history->list_of_move);
  free(board->history);
  board_free(board);
  free_tree(tree);

  return EXIT_SUCCESS;
}
