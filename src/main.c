#include "bb.h"
#include "board.h"
#include "bot.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include "uci.h"
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

void test_print_moves(Move *l, int count) {
  for (int i = 0; i < count; i++) {
    char *mv = move_to_algebric(l[i]);
    wprintf(L"%s ", mv);
    free(mv);
  }
  wprintf(L"\n");
}

bool player1 = true;
bool player2 = true;
Move move;
Undo undo;

int command(char *strmove, Board **board) {
  if (!strncmp(strmove, "init", 4)) {
    if (!strcmp(strmove, "initpp")) {
      player1 = true;
      player2 = true;
    } else if (!strcmp(strmove, "initbp")) {
      player1 = false;
      player2 = true;
    } else if (!strcmp(strmove, "initpb")) {
      player1 = true;
      player2 = false;
    } else if (!strcmp(strmove, "initbb")) {
      player1 = false;
      player2 = false;
    }
    free((*board)->history->list_of_move);
    free((*board)->history);
    board_free(*board);
    *board = board_init();
    threat_board_update(*board);
    return 2;
  } else if (!strncmp(strmove, "pbb", 3)) {
    board_bb_info(*board);
    return -1;
  } else if (!strncmp(strmove, "ppm", 3)) {
    Move moves[MAX_MOVES];
    int count = move_possible(*board, moves);
    test_print_moves(moves, count);
    return -1;
  } else if (!strncmp(strmove, "exit", 4)) {
    free((*board)->history->list_of_move);
    free((*board)->history);
    board_free(*board);
    exit(EXIT_SUCCESS);
  } else {
    move = algebric_to_move(strmove, *board);
    int res = move_check_validity(*board, move.from, move.to);
    if (!res) {
      wprintf(L"Invalid move\n");
      return -1;
    }
    return move_make(*board, &move, &undo);
  }
}

int bot_turn(Board *board) {
  Move bot = choose(board);

  char *mv = move_to_algebric(bot);
  wprintf(L"%s\n", mv);
  free(mv);

  return move_make(board, &bot, &undo);
}

int main(void) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling
  bb_magic_init();

  Board *board = board_init();
  char strmove[15];
  int res;

  while (true) {
    board_info(board);
    if (player1) {
      wprintf(L"\nPlayer Turn white\n");
      res = -1;
      while (res) {
        scanf("%s", strmove);
        res = command(strmove, &board);
        if (res == 2)
          break;
      }
      if (res == 2) {
        continue;
      }
    } else {
      wprintf(L"\nBOT WHITE\n");
      res = bot_turn(board);
      if (res) {
        wprintf(L"Bot fail, bot dumb\n");
        while (res) {
          scanf("%s", strmove);
          res = command(strmove, &board);
        }
        continue;
      }
    }

    board_info(board);
    if (player2) {
      wprintf(L"\nPlayer Turn black\n");
      res = -1;
      while (res) {
        scanf("%s", strmove);
        res = command(strmove, &board);
        if (res == 2)
          break;
      }
      if (res == 2) {
        continue;
      }
    } else {
      wprintf(L"\nBOT BLACK\n");
      res = bot_turn(board);
      if (res) {
        wprintf(L"Bot fail, bot dumb\n");
        while (res) {
          scanf("%s", strmove);
          res = command(strmove, &board);
        }
        continue;
      }
    }
  }

  free(board->history->list_of_move);
  free(board->history);
  board_free(board);

  return EXIT_SUCCESS;
}
