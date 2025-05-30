#include "bb.h"
#include "board.h"
#include "bot.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include "uci.h"
#include <assert.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>

void test_move_check_validity_bishop(void) {
  wprintf(L"- move_check_validity_bishop\n");
  Board *board = board_init();
  board_empty(board);

  // Bishop checks
  board_set(board, 3 + 3 * 8, WHITE_BISHOP);
  board_set(board, 2 + 2 * 8, BLACK_PAWN);

  assert(!move_check_validity(board, 3 + 3 * 8, 0 + 0 * 8));
  assert(move_check_validity(board, 3 + 3 * 8, 2 + 4 * 8));
  assert(!move_check_validity(board, 3 + 3 * 8, 7 + -1 * 8));
  assert(!move_check_validity(board, 3 + 3 * 8, 3 + 3 * 8));
}

void test_move_check_validity_rook(void) {
  wprintf(L"- move_check_validity_rook\n");
  Board *board = board_init();
  board_empty(board);

  // Rook checks
  board_set(board, 3 + 5 * 8, WHITE_ROOK);
  board_set(board, 3 + 7 * 8, BLACK_KNIGHT);

  assert(move_check_validity(board, 3 + 5 * 8, 3 + 6 * 8));
  assert(move_check_validity(board, 3 + 5 * 8, 3 + 7 * 8));
  assert(move_check_validity(board, 3 + 5 * 8, 3 + 4 * 8));
  assert(move_check_validity(board, 3 + 5 * 8, 2 + 5 * 8));
  assert(move_check_validity(board, 3 + 5 * 8, 4 + 5 * 8));
  assert(move_check_validity(board, 3 + 5 * 8, 3 + 3 * 8));
  assert(!move_check_validity(board, 3 + 5 * 8, 4 + 6 * 8));
  assert(!move_check_validity(board, 3 + 5 * 8, 1 + 7 * 8));
  board_set(board, 3 + 6 * 8, BLACK_PAWN);
  assert(!move_check_validity(board, 3 + 5 * 8, 3 + 7 * 8));
}

void test_move_check_validity_knight(void) {
  wprintf(L"- move_check_validity_knight\n");
  Board *board = board_init();
  board_empty(board);

  // Knight checks
  board_set(board, 4 + 4 * 8, WHITE_KNIGHT);
  board_set(board, 2 + 5 * 8, BLACK_PAWN);
  board_set(board, 6 + 3 * 8, BLACK_KING);

  assert(move_check_validity(board, 4 + 4 * 8, 2 + 5 * 8));
  assert(move_check_validity(board, 4 + 4 * 8, 6 + 3 * 8));
  assert(move_check_validity(board, 4 + 4 * 8, 5 + 6 * 8));
  assert(move_check_validity(board, 4 + 4 * 8, 3 + 6 * 8));
  assert(move_check_validity(board, 4 + 4 * 8, 2 + 3 * 8));
  assert(move_check_validity(board, 4 + 4 * 8, 3 + 2 * 8));
  assert(move_check_validity(board, 4 + 4 * 8, 5 + 2 * 8));
  assert(move_check_validity(board, 4 + 4 * 8, 6 + 5 * 8));
  assert(!move_check_validity(board, 4 + 4 * 8, 4 + 5 * 8));
  assert(!move_check_validity(board, 4 + 4 * 8, 4 + 4 * 8));
  assert(!move_check_validity(board, 4 + 4 * 8, 7 + 7 * 8));
}

void test_move_check_validity_pawn(void) {
  wprintf(L"- move_check_validity_pawn\n");
  Board *board = board_init();
  board_empty(board);

  // Test white pawn moves
  board_set(board, 4 + 1 * 8, WHITE_PAWN);
  board_set(board, 5 + 2 * 8, BLACK_PAWN);

  threat_board_update(board);

  // Regular moves
  assert(move_check_validity(board, 4 + 1 * 8, 4 + 2 * 8)); // Single advance
  assert(move_check_validity(board, 4 + 1 * 8, 4 + 3 * 8)); // Double advance
  assert(move_check_validity(board, 4 + 1 * 8, 5 + 2 * 8)); // Capture

  // Invalid moves
  assert(!move_check_validity(board, 4 + 1 * 8, 4 + 0 * 8)); // Backward
  assert(!move_check_validity(board, 4 + 1 * 8, 3 + 2 * 8)); // Empty diagonal
  assert(!move_check_validity(board, 4 + 1 * 8, 4 + 4 * 8)); // Too far

  // Test blocked moves
  board_set(board, 4 + 2 * 8, BLACK_PAWN);
  assert(!move_check_validity(board, 4 + 1 * 8, 4 + 2 * 8)); // Blocked single
  assert(!move_check_validity(board, 4 + 1 * 8, 4 + 3 * 8)); // Blocked double

  // Test black pawn moves
  board_empty(board);
  board->color = BLACK;
  board_set(board, 4 + 6 * 8, BLACK_PAWN);
  board_set(board, 3 + 5 * 8, WHITE_PAWN);

  assert(move_check_validity(board, 4 + 6 * 8, 4 + 5 * 8)); // Single advance
  assert(move_check_validity(board, 4 + 6 * 8, 4 + 4 * 8)); // Double advance
  assert(move_check_validity(board, 4 + 6 * 8, 3 + 5 * 8)); // Capture

  // Test en passant
  board_empty(board);
  board_set(board, 4 + 4 * 8, WHITE_PAWN);
  board_set(board, 5 + 4 * 8, BLACK_PAWN);

  board->ep = 1ULL << (5 + 5 * 8);

  assert(move_check_validity(board, 4 + 4 * 8, 5 + 5 * 8));
  assert(!move_check_validity(board, 4 + 4 * 8, 3 + 5 * 8));
  assert(!move_check_validity(board, 4 + 4 * 8, 5 + 3 * 8));
}

void test_move_check_validity_king(void) {
  wprintf(L"- move_king\n");
  Board *board = board_init();
  board_empty(board);
  Undo undo;

  board_set(board, 3 + 4 * 8, WHITE_KING);
  Move move_data = {WHITE_KING, 3 + 4 * 8, 3 + 5 * 8};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 3 + 4 * 8) == EMPTY);
  assert(board_get(board, 3 + 5 * 8) == WHITE_KING);
  assert(board->color == BLACK);

  board_empty(board); // Queen-side castling
  board_set(board, 3 + 0 * 8, WHITE_KING);
  board_set(board, 0 + 0 * 8, WHITE_ROOK);
  board_set(board, 7 + 0 * 8, WHITE_ROOK);
  move_data = (Move){WHITE_KING, 3 + 0 * 8, 1 + 0 * 8};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 3 + 0 * 8) == EMPTY);
  assert(board_get(board, 1 + 0 * 8) == WHITE_KING);
  assert(board_get(board, 2 + 0 * 8) == WHITE_ROOK);

  board_empty(board); // King-side castling
  board_set(board, 3 + 0 * 8, WHITE_KING);
  board_set(board, 0 + 0 * 8, WHITE_ROOK);
  board_set(board, 7 + 0 * 8, WHITE_ROOK);
  move_data = (Move){WHITE_KING, 3 + 0 * 8, 5 + 0 * 8};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 3 + 0 * 8) == EMPTY);
  assert(board_get(board, 5 + 0 * 8) == WHITE_KING);
  assert(board_get(board, 4 + 0 * 8) == WHITE_ROOK);

  board_free(board);
}

void test_move_basic(void) {
  wprintf(L"- move_basic\n");
  Board *board = board_init();
  board_empty(board);

  Undo undo;

  board_set(board, 4 + 4 * 8, WHITE_ROOK);
  Move move_data = {WHITE_ROOK, 4 + 4 * 8, 4 + 6 * 8};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 4 + 4 * 8) == EMPTY);
  assert(board_get(board, 4 + 6 * 8) == WHITE_ROOK);
  assert(board->color == BLACK);
}

void test_move_capture(void) {
  wprintf(L"- move_capture\n");
  Board *board = board_init();
  board_empty(board);

  Undo undo;

  board_set(board, 3 + 3 * 8, WHITE_BISHOP);
  board_set(board, 5 + 5 * 8, BLACK_PAWN);
  Move move_data = {WHITE_BISHOP, 3 + 3 * 8, 5 + 5 * 8};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 3 + 3 * 8) == EMPTY);
  assert(board_get(board, 5 + 5 * 8) == WHITE_BISHOP);
}

void test_move_enpassant(void) {
  wprintf(L"- move_enpassant\n");
  Board *board = board_init();
  board_empty(board);

  Undo undo;

  board_set(board, 4 + 4 * 8, WHITE_PAWN);
  board_set(board, 5 + 4 * 8, BLACK_PAWN);
  Move last_move = {BLACK_PAWN, 5 + 6 * 8, 5 + 4 * 8};
  board_add_move(board, last_move);

  Move move_data = {WHITE_PAWN, 4 + 4 * 8, 5 + 5 * 8};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 4 + 4 * 8) == EMPTY);
  assert(board_get(board, 5 + 4 * 8) == EMPTY);
  assert(board_get(board, 5 + 5 * 8) == WHITE_PAWN);
}

void test_move_promotion(void) {
  wprintf(L"- move_promotion\n");
  Board *board = board_init();
  board_empty(board);

  Undo undo;

  board_set(board, 3 + 6 * 8, WHITE_PAWN);
  Move move_data = {WHITE_PAWN, 3 + 6 * 8, 3 + 7 * 8, WHITE_QUEEN};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 3 + 7 * 8) == WHITE_QUEEN);

  board->color = BLACK;
  board_set(board, 4 + 1 * 8, BLACK_PAWN);
  move_data = (Move){BLACK_PAWN, 4 + 1 * 8, 4 + 0 * 8, BLACK_QUEEN};
  move_make(board, &move_data, &undo);
  assert(board_get(board, 4 + 0 * 8) == BLACK_QUEEN);
}

void test_move_castling(void) {
  wprintf(L"- move_castling\n");
  Board *board = board_init();
  threat_board_update(board);

  Undo undo;
  move_make(board, &(Move){board_get(board, 1 + 1 * 8), 1 + 1 * 8, 1 + 2 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 6 + 7 * 8), 6 + 7 * 8, 7 + 5 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 1 + 0 * 8), 1 + 0 * 8, 2 + 2 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 6 + 6 * 8), 6 + 6 * 8, 6 + 5 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 2 + 0 * 8), 2 + 0 * 8, 0 + 2 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 5 + 7 * 8), 5 + 7 * 8, 6 + 6 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 3 + 1 * 8), 3 + 1 * 8, 3 + 3 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 6 + 6 * 8), 6 + 6 * 8, 5 + 5 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 3 + 0 * 8), 3 + 0 * 8, 1 + 0 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 4 + 7 * 8), 4 + 7 * 8, 5 + 7 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 7 + 1 * 8), 7 + 1 * 8, 7 + 2 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 5 + 7 * 8), 5 + 7 * 8, 6 + 6 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 7 + 0 * 8), 7 + 0 * 8, 7 + 1 * 8},
            &undo);
  move_make(board, &(Move){board_get(board, 3 + 7 * 8), 3 + 7 * 8, 5 + 7 * 8},
            &undo);
  assert(board_get(board, 5 + 7 * 8) == BLACK_KING);
  assert(board_get(board, 4 + 7 * 8) == BLACK_ROOK);
  assert(board_get(board, 2) == WHITE_ROOK);
  assert(board_get(board, 1) == WHITE_KING);
}

void test_rook_attacks(void) {
  wprintf(L"- rook_attacks\n");
  Bb occ = 0x0000000000000000;
  Bb attacks = bb_rook_attacks(occ, 0);
  assert(attacks == 0x1010101010101fe);

  occ = 0x00000FE4000000FF0;
  attacks = bb_rook_attacks(occ, 34);
  assert(attacks == 0x47b04040400);
}

void test_bishop_attacks(void) {
  wprintf(L"- bishop_attacks\n");
  Bb occ = 0x0000000000000000;
  Bb attacks = bb_bishop_attacks(occ, 0);
  assert(attacks == 0x8040201008040200);

  occ = 0x0000000000000000;
  attacks = bb_bishop_attacks(occ, 34);
  assert(attacks == 0x20110a000a112040);

  occ = 0x00000FE4000000FF0;
  attacks = bb_bishop_attacks(occ, 36);
  assert(attacks == 0x280028448200);
}

void test_threat_check(void) {
  wprintf(L"- threat_check\n");
#ifdef MENACE
  Board *board = board_init();
  board_empty(board);

  board_set(board, 3 + 3 * 8, WHITE_KING);
  board_set(board, 2 + 2 * 8, BLACK_PAWN);
  board_set(board, 4 + 4 * 8, WHITE_BISHOP);
  board_set(board, 5 + 5 * 8, BLACK_PAWN);
  board_set(board, 3 + 5 * 8, BLACK_KNIGHT);

  board->color = BLACK;

  threat_board_update(board);
  assert(threat_check(board));

  board_set(board, 5 + 5 * 8, EMPTY);

  threat_board_update(board);
  assert(!threat_check(board));

  board_empty(board);
  board_set(board, 4 + 4 * 8, WHITE_KING);
  board_set(board, 5 + 5 * 8, BLACK_PAWN);
  board_set(board, 3 + 5 * 8, BLACK_KNIGHT);
  board_set(board, 3 + 3 * 8, BLACK_KING);

  threat_board_update(board);
  assert(threat_check(board));

  board_set(board, 4 + 4 * 8, EMPTY);
  board_set(board, 4 + 4 * 8, WHITE_ROOK);
  board_set(board, 1 + 4 * 8, WHITE_KING);

  threat_board_update(board);
  assert(!threat_check(board));

  board->color = BLACK;
  threat_board_update(board);
  assert(threat_check(board));
#else
  Board *board = board_init();
  board_empty(board);

  board_set(board, 4 + 4 * 8, WHITE_KING);
  board_set(board, 4 + 6 * 8, BLACK_ROOK);
  board_set(board, 2 + 2 * 8, BLACK_BISHOP);
  board_set(board, 5 + 5 * 8, BLACK_KNIGHT);

  threat_board_update(board);
  assert(!threat_check(board));

  board->color = BLACK;

  assert(threat_check(board));

  board->color = WHITE;

  board_empty(board);
  board_set(board, 4 + 4 * 8, WHITE_KING);
  board_set(board, 5 + 5 * 8, BLACK_ROOK);
  board_set(board, 3 + 3 * 8, BLACK_BISHOP);
  board_set(board, 2 + 2 * 8, BLACK_KNIGHT);

  threat_board_update(board);
  assert(!threat_check(board));

  board->color = BLACK;

  assert(threat_check(board));

  board->color = WHITE;

  board_set(board, 4 + 4 * 8, EMPTY);
  board_set(board, 4 + 4 * 8, WHITE_ROOK);
  board_set(board, 1 + 4 * 8, WHITE_KING);

  threat_board_update(board);
  assert(!threat_check(board));

  board->color = BLACK;
  threat_board_update(board);
  assert(threat_check(board));
#endif
}

void test_threat(void) { test_threat_check(); }

void test_move(void) {
  // The queen does not have to be tested because it is a rook/bishop
  test_move_check_validity_bishop();
  test_move_check_validity_rook();
  test_move_check_validity_knight();
  test_move_check_validity_pawn();
  test_move_check_validity_king();
  test_move_basic();
  test_move_capture();
  test_move_enpassant();
  test_move_promotion();
  test_move_castling();
}

void test_bb(void) {
  test_rook_attacks();
  test_bishop_attacks();
}

int main(void) {
  setlocale(LC_ALL, ""); // Enable Unicode Handling

  bb_magic_init();

  test_move();
  test_bb();
  test_threat();

  wprintf(L"Everything looks good\n");
  return EXIT_SUCCESS;
}
