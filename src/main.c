#include "board.h"
#include "move.h"
#include "types.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling

  Board *board = board_init();
  int orig[2], dest[2];
  while (true) {
    board_info(board);
    scanf("%d %d, %d %d", &orig[0], &orig[1], &dest[0], &dest[1]);
    move(board, (Move){board_get(board, orig[0] + orig[1] * 8),
                       {orig[0], orig[1]},
                       {dest[0], dest[1]}});
  }

  return EXIT_SUCCESS;
}
