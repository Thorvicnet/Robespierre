#include "board.h"
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling

  Board *board = board_init();
  board_print(board);
  board_set(board, 0, WHITE_KING);
  board_print(board);
  board_free(board);
  return EXIT_SUCCESS;
}
