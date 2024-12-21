#include "board.h"
#include "move.h"
#include "types.h"
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

void test_move_check_validity_knight(void) {
  wprintf(L"- move_check_validity_knight\n");
  Board *board = board_init();
  board_empty(board);

  // Knight checks
  board_set(board, 4 + 4 * 8, WHITE_KNIGHT);
  board_set(board, 2 + 5 * 8, BLACK_PAWN);
  board_set(board, 6 + 3 * 8, BLACK_KING);

  assert(move_check_validity(board, (int[]){4, 4}, (int[]){2, 5}));
  assert(move_check_validity(board, (int[]){4, 4}, (int[]){6, 3}));
  assert(move_check_validity(board, (int[]){4, 4}, (int[]){5, 6}));
  assert(move_check_validity(board, (int[]){4, 4}, (int[]){3, 6}));
  assert(move_check_validity(board, (int[]){4, 4}, (int[]){2, 3}));
  assert(move_check_validity(board, (int[]){4, 4}, (int[]){3, 2}));
  assert(move_check_validity(board, (int[]){4, 4}, (int[]){5, 2}));
  assert(move_check_validity(board, (int[]){4, 4}, (int[]){6, 5}));
  assert(!move_check_validity(board, (int[]){4, 4}, (int[]){4, 5}));
  assert(!move_check_validity(board, (int[]){4, 4}, (int[]){4, 4}));
  assert(!move_check_validity(board, (int[]){4, 4}, (int[]){7, 7}));
}

void test_move_check_validity_pawn(void) {
  wprintf(L"- move_check_validity_pawn\n");
  Board *board = board_init();

  // Test white pawn moves
  board_set(board, 4 + 1 * 8, WHITE_PAWN);
  board_set(board, 5 + 2 * 8, BLACK_PAWN);

  // Regular moves
  assert(move_check_validity(board, (int[]){4, 1},
                             (int[]){4, 2})); // Single advance
  assert(move_check_validity(board, (int[]){4, 1},
                             (int[]){4, 3})); // Double advance
  assert(move_check_validity(board, (int[]){4, 1}, (int[]){5, 2})); // Capture

  // Invalid moves
  assert(!move_check_validity(board, (int[]){4, 1}, (int[]){4, 0})); // Backward
  assert(!move_check_validity(board, (int[]){4, 1},
                              (int[]){3, 2})); // Empty diagonal
  assert(!move_check_validity(board, (int[]){4, 1}, (int[]){4, 4})); // Too far

  // Test blocked moves
  board_set(board, 4 + 2 * 8, BLACK_PAWN);
  assert(!move_check_validity(board, (int[]){4, 1},
                              (int[]){4, 2})); // Blocked single
  assert(!move_check_validity(board, (int[]){4, 1},
                              (int[]){4, 3})); // Blocked double

  // Test black pawn moves
  board_empty(board);
  board->color = BLACK;
  board_set(board, 4 + 6 * 8, BLACK_PAWN);
  board_set(board, 3 + 5 * 8, WHITE_PAWN);

  assert(move_check_validity(board, (int[]){4, 6},
                             (int[]){4, 5})); // Single advance
  assert(move_check_validity(board, (int[]){4, 6},
                             (int[]){4, 4})); // Double advance
  assert(move_check_validity(board, (int[]){4, 6}, (int[]){3, 5})); // Capture

  // Test en passant
  board_empty(board);
  board->color = WHITE;
  board_set(board, 4 + 4 * 8, WHITE_PAWN);
  board_set(board, 5 + 4 * 8, BLACK_PAWN);

  Move last_move = {BLACK_PAWN, {5, 6}, {5, 4}};
  board_add_move(board, last_move);

  board_list_moves(board);
  board_info(board);

  assert(move_check_validity(board, (int[]){4, 4},
                             (int[]){5, 5}));
  assert(!move_check_validity(board, (int[]){4, 4}, (int[]){3, 5}));
  assert(!move_check_validity(board, (int[]){4, 4}, (int[]){5, 3}));
}

void test_move(void) {
  // The queen does not have to be tested because it is a rook/bishop
  test_move_check_validity_bishop();
  test_move_check_validity_rook();
  test_move_check_validity_knight();
  test_move_check_validity_pawn();
}

int main(void) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling

  test_move();

  wprintf(L"Everything looks good\n");
  return EXIT_SUCCESS;
}
