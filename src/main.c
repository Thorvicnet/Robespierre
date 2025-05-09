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
#include <string.h>
#include <wchar.h>

void print_moves(Move *l, int count) {
  for (int i = 0; i < count; i++) {
    char *mv = move_to_algebric(l[i]);
    wprintf(L"%s ", mv);
    free(mv);
  }
  wprintf(L"\n");
}

bool player1 = true;
bool player2 = true;

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
    print_moves(moves, count);
    return -1;
  } else if (!strncmp(strmove, "efen", 4)) {
    char *fen = create_fen_from_board(*board);
    wprintf(L"Le fen de cette postion : %s\n", fen);
    free(fen);
    return -1;
  } else if (!strncmp(strmove, "ifen", 4)) {
    char fen[80];
    wprintf(L"Enter fen : ");

    scanf("%s", fen);
    int ret = transform_board_from_fen(fen, *board);

    while (1) {
      wprintf(L"\nenter mode (bb/pb/bp/pp) : ");
      scanf("%s", fen);
      if (strncmp(fen, "bb", 2) == 0) {
        player1 = false;
        player2 = false;
        break;
      } else if (strncmp(fen, "pb", 2) == 0) {
        player1 = true;
        player2 = false;
        break;
      } else if (strncmp(fen, "bp", 2) == 0) {
        player1 = false;
        player2 = true;
        break;
      } else if (strncmp(fen, "pp", 2) == 0) {
        player1 = true;
        player2 = true;
        break;
      }
    }

    if (ret == WHITE) {
      return 2;
    }
    return 0;
  } else if (!strncmp(strmove, "exit", 4)) {
    free((*board)->history->list_of_move);
    free((*board)->history);
    board_free(*board);
    free_tt();
    exit(EXIT_SUCCESS);
  } else {
    Move move;
    Undo undo;
    move = algebric_to_move(strmove, *board);
    int res = move_check_validity(*board, move.from, move.to);
    if (!res) {
      wprintf(L"Invalid move\n");
      return -1;
    }
    board_bb_info(*board);
    return move_make(*board, &move, &undo);
  }
}

void bot_turn(Board *board) {
  Move best_move;
  double max_time = 3.0;
  iterative_deepening(board, &best_move, max_time);

  Undo undo;
  move_make(board, &best_move, &undo);

  char *mv = move_to_algebric(best_move);
  wprintf(L"%s\n", mv);
  free(mv);
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  bb_magic_init();

  Board *board = board_init();

  if (argc > 1 && strcmp(argv[1], "nouci") == 0) {
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
        bot_turn(board);
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
        bot_turn(board);
      }
    }
  } else {
    uci_loop(board); // starts uci mode
  }

  free(board->history->list_of_move);
  free(board->history);
  board_free(board);
  free_tt();
  return EXIT_SUCCESS;
}
