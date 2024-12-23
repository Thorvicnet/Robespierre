#include "board.h"

const wchar_t piece_chars[] = {
    // Comment: the chars are too "big" for a normal char type
    [EMPTY] = L' ',        [WHITE_PAWN] = L'♟', [WHITE_KNIGHT] = L'♞',
    [WHITE_BISHOP] = L'♝', [WHITE_ROOK] = L'♜', [WHITE_QUEEN] = L'♛',
    [WHITE_KING] = L'♚',   [BLACK_PAWN] = L'♙', [BLACK_KNIGHT] = L'♘',
    [BLACK_BISHOP] = L'♗', [BLACK_ROOK] = L'♖', [BLACK_QUEEN] = L'♕',
    [BLACK_KING] = L'♔',
};

Board *board_init(void) {
  // Returns a board in the default position
  Board *board = (Board *)malloc(sizeof(Board));
  if (board == NULL) {
    printf("Your memory is doomed...");
    exit(EXIT_FAILURE);
  }

  board->color = WHITE;
  board->history = stack_create();
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
  tab[6 + 8 * 7] = BLACK_KNIGHT;

  tab[2] = WHITE_BISHOP;
  tab[5] = WHITE_BISHOP;
  tab[2 + 8 * 7] = BLACK_BISHOP;
  tab[5 + 8 * 7] = BLACK_BISHOP;

  tab[3] = WHITE_KING;
  tab[4] = WHITE_QUEEN;
  tab[3 + 8 * 7] = BLACK_KING;
  tab[4 + 8 * 7] = BLACK_QUEEN;

  bb_board_init(board);

  return board;
}

void board_empty(Board *board) {
  // Empties the board (for testing purposes)
  for (int i = 0; i < 64; i++) {
    board_set(board, i, EMPTY);
  }
  board->color = WHITE;
  board->history->last_move = 0;
  board->all = 0;
  board->white = 0;
  board->black = 0;
  board->white_pawns = 0;
  board->white_knights = 0;
  board->white_bishops = 0;
  board->white_rooks = 0;
  board->white_queens = 0;
  board->white_kings = 0;
  board->black_pawns = 0;
  board->black_knights = 0;
  board->black_bishops = 0;
  board->black_rooks = 0;
  board->black_queens = 0;
  board->black_kings = 0;
}

void board_free(Board *board) {
  // Frees the board from memory (very simple ATM)
  free(board);
}

void board_set(Board *board, int sq, int piece) {
  // TODO: Is it really performant to clear all the bitboards, is it possible to do better ?
  Bb bit = 1ULL << sq;
  board->all &= ~bit;
  board->white &= ~bit;
  board->black &= ~bit;
  board->white_pawns &= ~bit;
  board->white_knights &= ~bit;
  board->white_bishops &= ~bit;
  board->white_rooks &= ~bit;
  board->white_queens &= ~bit;
  board->white_kings &= ~bit;
  board->black_pawns &= ~bit;
  board->black_knights &= ~bit;
  board->black_bishops &= ~bit;
  board->black_rooks &= ~bit;
  board->black_queens &= ~bit;
  board->black_kings &= ~bit;

  board->squares[sq] = piece; 
  board->all |= bit;
  if (COLOR(piece) == WHITE) {
    board->white |= bit;
    switch (piece) {
      case WHITE_PAWN: board->white_pawns |= bit; break;
      case WHITE_KNIGHT: board->white_knights |= bit; break;
      case WHITE_BISHOP: board->white_bishops |= bit; break;
      case WHITE_ROOK: board->white_rooks |= bit; break;
      case WHITE_QUEEN: board->white_queens |= bit; break;
      case WHITE_KING: board->white_kings |= bit; break;
    }
  } else if (COLOR(piece) == BLACK) {
    board->black |= bit;
    switch (piece) {
      case BLACK_PAWN: board->black_pawns |= bit; break;
      case BLACK_KNIGHT: board->black_knights |= bit; break;
      case BLACK_BISHOP: board->black_bishops |= bit; break;
      case BLACK_ROOK: board->black_rooks |= bit; break;
      case BLACK_QUEEN: board->black_queens |= bit; break;
      case BLACK_KING: board->black_kings |= bit; break;
    }
  }
 }

int board_get(Board *board, int sq) { return board->squares[sq]; }

Move board_last_move(Board *board) { return stack_peek(board->history); }

void board_add_move(Board *board, Move move) {
  stack_push(board->history, move);
}

void board_print(Board *board) {
  // Prints the board to stdout using chess using chars from U+2654 to U+265F
  for (int li = 0; li < 8; li++) {
    for (int col = 0; col < 8; col++) {
      wchar_t pchar = piece_chars[board->squares[col + 8 * li]];
      if ((li + col) % 2 == 0) {
        wprintf(L"\033[48;5;230m %lc \033[0m", pchar);
      } else {
        wprintf(L"\033[48;5;94m %lc \033[0m", pchar);
      }
    }
    wprintf(L"\n");
  }
}

void board_info(Board *board) {
  board_print(board);
  wprintf(L"%s to play, Moves: %d\n", board->color == WHITE ? "WHITE" : "BLACK",
          board->history->last_move);
}

void board_list_moves(Board *board) {
  // Lists all the moves in the history stack
  Stack *history = board->history;
  for (int i = 0; i < history->last_move; i++) {
    Move move = history->list_of_move[i];
    wprintf(L"%d: %d %d -> %d %d ( %lc )\n", i, move.orig[0], move.orig[1],
            move.dest[0], move.dest[1], piece_chars[move.piece]);
  }
}
