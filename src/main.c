#include "board.h"
#include "move.h"
#include "types.h"
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling

  Board *board = board_init();
  board_empty(board);
  board_set(board, 3 + 3 * 8, WHITE_BISHOP);
  board_set(board, 2 + 2 * 8, BLACK_PAWN);
  board_info(board);
  wprintf(L"%d", move_check_validity(board, (int[]){3, 3}, (int[]){0, 0}));

  return EXIT_SUCCESS;
}
