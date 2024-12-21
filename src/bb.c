#include "bb.h"

void bb_board_init(Board *board) {
  board->white_pawns = 0x000000000000FF00ULL;
  board->white_rooks = 0x0000000000000081ULL;
  board->white_knights = 0x0000000000000042ULL;
  board->white_bishops = 0x0000000000000024ULL;
  board->white_queens = 0x0000000000000008ULL;
  board->white_kings = 0x0000000000000010ULL;

  board->black_pawns = 0x00FF000000000000ULL;
  board->black_rooks = 0x8100000000000000ULL;
  board->black_knights = 0x4200000000000000ULL;
  board->black_bishops = 0x2400000000000000ULL;
  board->black_queens = 0x0800000000000000ULL;
  board->black_kings = 0x1000000000000000ULL;

  board->white = board->white_pawns | board->white_rooks |
                 board->white_knights | board->white_knights |
                 board->white_bishops | board->white_queens |
                 board->white_kings;
  board->black = board->black_pawns | board->black_rooks |
                 board->black_knights | board->black_knights |
                 board->black_bishops | board->black_queens |
                 board->black_kings;

  board->all = board->white | board->black;
}

void print_bitboard(bb b) {
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file; // Map rank/file to square index
      wprintf(L"%c ", (b & (1ULL << square)) ? '1' : '.');
    }
    wprintf(L"\n");
  }
  wprintf(L"\n");
}
