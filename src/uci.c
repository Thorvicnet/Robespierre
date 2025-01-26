#include "uci.h"
#include "board.h"
#include "types.h"
#include <string.h>
#include <wchar.h>

char *move_to_algebric(Move move) {
  char alphabet[] = "hgfedcba";
  char number[] = "12345678";

  char *ret = (char *)malloc(6 * sizeof(char));
  if (ret == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }

  snprintf(ret, 3, "%c%c", alphabet[move.from & 7], number[move.from >> 3]);
  snprintf(ret + 2, 3, "%c%c", alphabet[move.to & 7], number[move.to >> 3]);

  if ((PIECE(move.promote)) == QUEEN)
    strcat(ret, "q");
  else if ((PIECE(move.promote)) == BISHOP)
    strcat(ret, "b");
  else if ((PIECE(move.promote)) == ROOK)
    strcat(ret, "r");
  else if ((PIECE(move.promote)) == KNIGHT)
    strcat(ret, "n");

  return ret;
}

Move algebric_to_move(char *ch, Board *board) {
  int orig[2] = {(int)7 - (ch[0] - 97), (int)ch[1] - 49};

  int dest[2] = {(int)7 - (ch[2] - 97), (int)ch[3] - 49};
  int promot = EMPTY;
  if (strlen(ch) > 4) {
    if (ch[4] == 'q')
      promot = (QUEEN | board->color);
    if (ch[4] == 'b')
      promot = (BISHOP | board->color);
    if (ch[4] == 'r')
      promot = (ROOK | board->color);
    if (ch[4] == 'n')
      promot = (KNIGHT | board->color);
  }

  return (Move){board_get(board, orig[0] + 8 * orig[1]), orig[0] + orig[1] * 8,
                dest[0] + dest[1] * 8, promot};
}

int transform_board_from_fen(char *fen, Board *board) {
  board_empty(board);
  int i = 0;
  int j = 0;
  for (; i < 64; j++) {
    char c = fen[j];
    switch (c) {
    case '1':
      i += 1;
      break;
    case '2':
      i += 2;
      break;
    case '3':
      i += 3;
      break;
    case '4':
      i += 4;
      break;
    case '5':
      i += 5;
      break;
    case '6':
      i += 6;
      break;
    case '7':
      i += 7;
      break;
    case '8':
      i += 8;
      break;

    case 'n':
      board_set(board, 63 - i++, BLACK_KNIGHT);
      break;
    case 'N':
      board_set(board, 63 - i++, WHITE_KNIGHT);
      break;
    case 'p':
      board_set(board, 63 - i++, BLACK_PAWN);
      break;
    case 'P':
      board_set(board, 63 - i++, WHITE_PAWN);
      break;
    case 'r':
      board_set(board, 63 - i++, BLACK_ROOK);
      break;
    case 'R':
      board_set(board, 63 - i++, WHITE_ROOK);
      break;
    case 'b':
      board_set(board, 63 - i++, BLACK_BISHOP);
      break;
    case 'B':
      board_set(board, 63 - i++, WHITE_BISHOP);
      break;
    case 'q':
      board_set(board, 63 - i++, BLACK_QUEEN);
      break;
    case 'Q':
      board_set(board, 63 - i++, WHITE_QUEEN);
      break;
    case 'k':
      board_set(board, 63 - i++, BLACK_KING);
      break;
    case 'K':
      board_set(board, 63 - i++, WHITE_KING);
      break;
    }
  }
  j += 2;
  if (fen[j] == 'b') {
    board->color = BLACK;
    return BLACK;
  }
  return WHITE;
}

char *create_fen_from_board(Board *board) {
  char *fen = (char *)malloc(sizeof(char) * 100);
  if (fen == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }

  int empty_case = 0;
  int p = 0;
  for (int i = 7; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
      int piece = board_get(board, j + i * 8);
      if (piece == EMPTY) {
        empty_case += 1;
      } else {
        if (empty_case != 0) {
          fen[p] = (char)(empty_case + 48);
          p += 1;
          empty_case = 0;
        }
        switch (piece) {
        case BLACK_KNIGHT:
          fen[p++] = 'n';
          break;
        case WHITE_KNIGHT:
          fen[p++] = 'N';
          break;
        case BLACK_ROOK:
          fen[p++] = 'r';
          break;
        case WHITE_ROOK:
          fen[p++] = 'R';
          break;
        case BLACK_BISHOP:
          fen[p++] = 'b';
          break;
        case WHITE_BISHOP:
          fen[p++] = 'B';
          break;
        case BLACK_PAWN:
          fen[p++] = 'p';
          break;
        case WHITE_PAWN:
          fen[p++] = 'P';
          break;
        case BLACK_QUEEN:
          fen[p++] = 'q';
          break;
        case WHITE_QUEEN:
          fen[p++] = 'Q';
          break;
        case BLACK_KING:
          fen[p++] = 'k';
          break;
        case WHITE_KING:
          fen[p++] = 'K';
          break;
        }
      }
    }
    if (empty_case != 0) {
      fen[p] = (char)(empty_case + 48);
      p += 1;
      empty_case = 0;
    }
    if (i != 0)
      fen[p++] = '/';
  }
  fen[p++] = ' ';
  if (board->color == BLACK)
    fen[p++] = 'b';
  if (board->color == WHITE)
    fen[p++] = 'w';
  for (; p < 100; p++)
    fen[p] = ' ';
  return fen;
}

void uci_loop(Board *board) {
  char *line = NULL;
  size_t len = 0;

  setbuf(stdout, NULL);

  while (1) {
    if (getline(&line, &len, stdin) == -1) {
      wprintf(L"Error reading line\n");
      fflush(stdout);
      exit(EXIT_FAILURE);
    }

    line[strcspn(line, "\n")] = 0;

    if (strncmp(line, "ucinewgame", 10) == 0) {
      // Free current board and initialize a new one
      free(board->history->list_of_move);
      free(board->history);
      board_free(board);

      board = board_init();
      threat_board_update(board);

    } else if (strncmp(line, "uci", 3) == 0) {
      // Print engine info
      wprintf(L"id name %s\n", ENGINE_NAME);
      wprintf(L"id author %s\n", ENGINE_AUTHOR);
      wprintf(L"option name Hash type spin default 1 min 1 max 1024\n");
      wprintf(L"option name Threads type spin default 1 min 1 max 128\n");
      wprintf(
          L"option name Move Overhead type spin default 100 min 0 max 5000\n");
      wprintf(L"uciok\n");
      fflush(stdout);

    } else if (strncmp(line, "isready", 7) == 0) {
      wprintf(L"readyok\n");
      fflush(stdout);

    } else if (strncmp(line, "position", 8) == 0) {
      char *token = strtok(line, " ");
      token = strtok(NULL, " ");

      if (token && strcmp(token, "startpos") == 0) {
        transform_board_from_fen(START_FEN, board);
      } else if (token && strcmp(token, "fen") == 0) {
        char fen[128] = "";
        int fen_parts = 0;
        while ((token = strtok(NULL, " ")) != NULL &&
               strcmp(token, "moves") != 0) {
          strcat(fen, token);
          strcat(fen, " ");
          if (++fen_parts >= 6)
            break;
        }
        transform_board_from_fen(fen, board);
      }

      // Process moves in the given position
      token = strtok(NULL, " ");
      if (token && strcmp(token, "moves") == 0) {
        while ((token = strtok(NULL, " ")) != NULL) {
          Move move = algebric_to_move(token, board);
          if (!move_check_validity(board, move.from, move.to)) {
            wprintf(L"Invalid move in sequence: %s\n", token);
            continue;
          }
          Undo undo;
          move_make(board, &move, &undo);
          threat_board_update(board);
        }
      }

    } else if (strncmp(line, "go", 2) == 0) {
      // Use iterative deepening to find the best move
      Move bot_move;
      double max_time = 3.0; // Default time for decision
      iterative_deepening(board, &bot_move, max_time);

      // Output the best move
      char *mv = move_to_algebric(bot_move);
      wprintf(L"bestmove %s\n", mv);
      fflush(stdout);
      free(mv);

      // Apply the bot's move
      Undo undo;
      move_make(board, &bot_move, &undo);
      threat_board_update(board);

    } else if (strncmp(line, "quit", 4) == 0 || strncmp(line, "stop", 4) == 0) {
      break;
    }
  }

  free(line);
}
