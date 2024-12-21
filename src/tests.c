#include "board.h"
#include "move.h"
#include <assert.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>

void test_move_check_validity_bishop(void) {
  wprintf(L"- move_check_validity_bishop\n");
  Board *board = board_init();
  board_empty(board);

  // Bishop checks
  board_set(board, 3 + 3 * 8, WHITE_BISHOP);
  board_set(board, 2 + 2 * 8, BLACK_PAWN);

  assert(!move_check_validity(board, (int[]){3, 3}, (int[]){0, 0}));
  assert(move_check_validity(board, (int[]){3, 3}, (int[]){2, 4}));
  assert(!move_check_validity(board, (int[]){3, 3}, (int[]){7, -1}));
  assert(!move_check_validity(board, (int[]){3, 3}, (int[]){3, 3}));
}

void test_move_check_validity_rook(void) {
  wprintf(L"- move_check_validity_rook\n");
  Board *board = board_init();
  board_empty(board);

  // Rook checks
  board_set(board, 3 + 5 * 8, WHITE_ROOK);
  board_set(board, 3 + 7 * 8, BLACK_KNIGHT);

  assert(move_check_validity(board, (int[]){3, 5}, (int[]){3, 6}));
  assert(move_check_validity(board, (int[]){3, 5}, (int[]){3, 7}));
  assert(move_check_validity(board, (int[]){3, 5}, (int[]){3, 4}));
  assert(move_check_validity(board, (int[]){3, 5}, (int[]){2, 5}));
  assert(move_check_validity(board, (int[]){3, 5}, (int[]){4, 5}));
  assert(move_check_validity(board, (int[]){3, 5}, (int[]){3, 3}));
  assert(!move_check_validity(board, (int[]){3, 5}, (int[]){4, 6}));
  assert(!move_check_validity(board, (int[]){3, 5}, (int[]){1, 7}));
  board_set(board, 3 + 6 * 8, BLACK_PAWN);
  assert(!move_check_validity(board, (int[]){3, 5}, (int[]){3, 7}));
}

void test_move(void) {
  test_move_check_validity_bishop();
  test_move_check_validity_rook();
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling

  test_move();

  wprintf(L"Everything looks good\n");
  return EXIT_SUCCESS;
}
