#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

const wchar_t piece_chars[] = {
    // Comment: the chars are too "big" for a normal char type
    [EMPTY] = ' ',         [WHITE_PAWN] = L'♙', [WHITE_KNIGHT] = L'♘',
    [WHITE_BISHOP] = L'♗', [WHITE_ROOK] = L'♖', [WHITE_QUEEN] = L'♕',
    [WHITE_KING] = L'♔',   [BLACK_PAWN] = L'♟', [BLACK_KNIGHT] = L'♞',
    [BLACK_BISHOP] = L'♝', [BLACK_ROOK] = L'♜', [BLACK_QUEEN] = L'♛',
    [BLACK_KING] = L'♚',
};

Board *board_init() {
  // Returns a board in the default position
  Board *board = (Board *)malloc(sizeof(Board));
  if (board == NULL) {
    printf("Your memory is doomed...");
    exit(EXIT_FAILURE);
  }
  int *tab = board->squares;

  for (int i = 0; i < 8; i++) {
    tab[i + 8] = WHITE_PAWN;
    tab[i + 8 * 6] = BLACK_PAWN;
  }

  tab[0] = WHITE_ROOK;
  tab[7] = WHITE_ROOK;
  tab[8 * 7] = BLACK_ROOK;
  tab[7 + 8 * 7] = BLACK_ROOK;

  tab[1] = WHITE_KNIGHT;
  tab[6] = WHITE_KNIGHT;
  tab[1 + 8 * 7] = BLACK_KNIGHT;
  tab[7 + 8 * 7] = BLACK_KNIGHT;

  tab[2] = WHITE_BISHOP;
  tab[5] = WHITE_BISHOP;
  tab[2 + 8 * 7] = BLACK_BISHOP;
  tab[6 + 8 * 7] = BLACK_BISHOP;

  tab[3] = WHITE_KING;
  tab[4] = WHITE_QUEEN;
  tab[3 + 8 * 7] = BLACK_KING;
  tab[4 + 8 * 7] = BLACK_QUEEN;

  return board;
}

void board_free(Board *board) {
  // Frees the board from memory (very simple ATM)
  free(board);
}

void board_set(Board *board, int sq, int piece) { board->squares[sq] = piece; }

int board_get(Board *board, int sq) { return board->squares[sq]; }

void board_print(Board *board) {
  // Prints the board to stdout using chess using chars from U+2654 to U+265F
  for (int li = 0; li < 8; li++) {
    for (int col = 0; col < 8; col++) {
      wprintf(L"%c", piece_chars[board->squares[col + 8 * li]]);
    }
    printf("\n");
  }
}
