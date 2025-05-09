#include "board.h"

const wchar_t piece_chars[] = {
    // Comment: the chars are too "big" for a normal char type
    [EMPTY] = L' ', [PAWN] = L'♟',  [KNIGHT] = L'♞', [BISHOP] = L'♝',
    [ROOK] = L'♜',  [QUEEN] = L'♛', [KING] = L'♚'};

Board *board_init(void) {
  // Returns a board pointer to a board in the default position
  Board *board = (Board *)malloc(sizeof(Board));
  if (board == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }

  board->color = WHITE;
  board->history = stack_create();
  uint8_t *tab = board->squares;

  // Castling and En passant flags
  board->castle = 0;
  board->ep = 0ULL;

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

  for (int i = 16; i < 48; i++) {
    tab[i] = EMPTY;
  }

  bb_board_init(board);

  return board;
}

void board_empty(Board *board) {
  // Empties the board (for testing purposes) and sets the color to WHITE
  for (int i = 0; i < 64; i++) {
    board_set(board, i, EMPTY);
  }
  board->color = WHITE;
  board->history->last_move = 0;
  board->castle = 0;
  board->ep = 0ULL;
  bb_board_empty(board);
}

void board_sync_bb(Board *board) {
  // Synchronizes the bitboards with the squares (for testing purposes, the move
  // function should already keep them in sync)
  for (int i = 0; i < 64; i++) {
    switch (board->squares[i]) {
    case WHITE_PAWN:
      board->white_pawns |= 1ULL << i;
      break;
    case WHITE_KNIGHT:
      board->white_knights |= 1ULL << i;
      break;
    case WHITE_BISHOP:
      board->white_bishops |= 1ULL << i;
      break;
    case WHITE_ROOK:
      board->white_rooks |= 1ULL << i;
      break;
    case WHITE_QUEEN:
      board->white_queens |= 1ULL << i;
      break;
    case WHITE_KING:
      board->white_kings |= 1ULL << i;
      break;
    case BLACK_PAWN:
      board->black_pawns |= 1ULL << i;
      break;
    case BLACK_KNIGHT:
      board->black_knights |= 1ULL << i;
      break;
    case BLACK_BISHOP:
      board->black_bishops |= 1ULL << i;
      break;
    case BLACK_ROOK:
      board->black_rooks |= 1ULL << i;
      break;
    case BLACK_QUEEN:
      board->black_queens |= 1ULL << i;
      break;
    case BLACK_KING:
      board->black_kings |= 1ULL << i;
      break;
    }
  }
}

Board *board_copy(Board *board) {
  // Copy a board but keeps the same history pointer
  Board *new_board = malloc(sizeof(Board));
  if (new_board == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }

  new_board->all = board->all;
  new_board->white = board->white;
  new_board->black = board->black;
  new_board->white_pawns = board->white_pawns;
  new_board->white_knights = board->white_knights;
  new_board->white_bishops = board->white_bishops;
  new_board->white_rooks = board->white_rooks;
  new_board->white_queens = board->white_queens;
  new_board->white_kings = board->white_kings;
  new_board->black_pawns = board->black_pawns;
  new_board->black_knights = board->black_knights;
  new_board->black_bishops = board->black_bishops;
  new_board->black_rooks = board->black_rooks;
  new_board->black_queens = board->black_queens;
  new_board->black_kings = board->black_kings;

  memcpy(new_board->squares, board->squares, 64 * sizeof(int));
  new_board->color = board->color;

  new_board->white_threat = board->white_threat;
  new_board->black_threat = board->black_threat;
  new_board->history = board->history;

  new_board->castle = board->castle;
  new_board->ep = board->ep;

  return new_board;
}

void board_free(Board *board) {
  // Frees the board from memory (very simple ATM)
  free(board);
}

void board_set_empty(Board *board, int sq, int capture) {
  // Set sq of type capture to EMPTY
  Bb bit = 1ULL << sq;

  board->squares[sq] = EMPTY;

  board->all &= ~bit;
  switch (capture) {
  // White pieces
  case WHITE_PAWN:
    board->white &= ~bit;
    board->white_pawns &= ~bit;
    break;
  case WHITE_KNIGHT:
    board->white &= ~bit;
    board->white_knights &= ~bit;
    break;
  case WHITE_BISHOP:
    board->white &= ~bit;
    board->white_bishops &= ~bit;
    break;
  case WHITE_ROOK:
    board->white &= ~bit;
    board->white_rooks &= ~bit;
    break;
  case WHITE_QUEEN:
    board->white &= ~bit;
    board->white_queens &= ~bit;
    break;
  case WHITE_KING:
    board->white &= ~bit;
    board->white_kings &= ~bit;
    break;

  // Black pieces
  case BLACK_PAWN:
    board->black &= ~bit;
    board->black_pawns &= ~bit;
    break;
  case BLACK_KNIGHT:
    board->black &= ~bit;
    board->black_knights &= ~bit;
    break;
  case BLACK_BISHOP:
    board->black &= ~bit;
    board->black_bishops &= ~bit;
    break;
  case BLACK_ROOK:
    board->black &= ~bit;
    board->black_rooks &= ~bit;
    break;
  case BLACK_QUEEN:
    board->black &= ~bit;
    board->black_queens &= ~bit;
    break;
  case BLACK_KING:
    board->black &= ~bit;
    board->black_kings &= ~bit;
    break;
  }
}

void board_set(Board *board, int sq, int piece) {
  // TODO: Is it really performant to clear all the bitboards, is it possible to
  // do better ?

  int capture = board->squares[sq];
  Bb bit = 1ULL << sq;

  if (capture != EMPTY) {
    board->all &= ~bit;
    switch (capture) {
    // White pieces
    case WHITE_PAWN:
      board->white &= ~bit;
      board->white_pawns &= ~bit;
      break;
    case WHITE_KNIGHT:
      board->white &= ~bit;
      board->white_knights &= ~bit;
      break;
    case WHITE_BISHOP:
      board->white &= ~bit;
      board->white_bishops &= ~bit;
      break;
    case WHITE_ROOK:
      board->white &= ~bit;
      board->white_rooks &= ~bit;
      break;
    case WHITE_QUEEN:
      board->white &= ~bit;
      board->white_queens &= ~bit;
      break;
    case WHITE_KING:
      board->white &= ~bit;
      board->white_kings &= ~bit;
      break;

    // Black pieces
    case BLACK_PAWN:
      board->black &= ~bit;
      board->black_pawns &= ~bit;
      break;
    case BLACK_KNIGHT:
      board->black &= ~bit;
      board->black_knights &= ~bit;
      break;
    case BLACK_BISHOP:
      board->black &= ~bit;
      board->black_bishops &= ~bit;
      break;
    case BLACK_ROOK:
      board->black &= ~bit;
      board->black_rooks &= ~bit;
      break;
    case BLACK_QUEEN:
      board->black &= ~bit;
      board->black_queens &= ~bit;
      break;
    case BLACK_KING:
      board->black &= ~bit;
      board->black_kings &= ~bit;
      break;
    }
  }

  board->squares[sq] = piece;
  if (piece == EMPTY) {
    return;
  }
  board->all |= bit;
  if (COLOR(piece) == WHITE) {
    board->white |= bit;
    switch (piece) {
    case WHITE_PAWN:
      board->white_pawns |= bit;
      break;
    case WHITE_KNIGHT:
      board->white_knights |= bit;
      break;
    case WHITE_BISHOP:
      board->white_bishops |= bit;
      break;
    case WHITE_ROOK:
      board->white_rooks |= bit;
      break;
    case WHITE_QUEEN:
      board->white_queens |= bit;
      break;
    case WHITE_KING:
      board->white_kings |= bit;
      break;
    }
  } else {
    board->black |= bit;
    switch (piece) {
    case BLACK_PAWN:
      board->black_pawns |= bit;
      break;
    case BLACK_KNIGHT:
      board->black_knights |= bit;
      break;
    case BLACK_BISHOP:
      board->black_bishops |= bit;
      break;
    case BLACK_ROOK:
      board->black_rooks |= bit;
      break;
    case BLACK_QUEEN:
      board->black_queens |= bit;
      break;
    case BLACK_KING:
      board->black_kings |= bit;
      break;
    }
  }
}

inline int board_get(Board *board, int sq) { return board->squares[sq]; }

Move board_last_move(Board *board) { return stack_peek(board->history); }

void board_add_move(Board *board, Move move) {
  stack_push(board->history, move);
}

void board_print(Board *board) {
  // Prints the board to stdout using chess chars from U+2654 to U+265F
  for (int li = 0; li < 8; li++) {
    for (int col = 0; col < 8; col++) {
      int piece = board->squares[col + 8 * li];
      wchar_t pchar = piece_chars[PIECE(piece)];
      if ((li + col) % 2 == 0) {
        wprintf(L"\033[48;5;250m");
      } else {
        wprintf(L"\033[48;5;94m");
      }
      if (piece == EMPTY) {
        wprintf(L"   \033[0m");
      } else if (COLOR(piece) == WHITE) {
        wprintf(L"\033[97m %lc \033[0m", pchar);
      } else {
        wprintf(L"\033[30m %lc \033[0m", pchar);
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

void board_bb_info(Board *board) {
  // Prints all bitboards in a table
  wprintf(L"Bitboards:\n");

  wprintf(L"All       White     Black     WThreat   BThreat\n");

  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->all & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white_threat & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black_threat & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"\n");
  }

  wprintf(L"\nWhite Pieces:\n");
  wprintf(L"Pawns     Knights   Bishops   Rooks     Queens    Kings\n");

  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white_pawns & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white_knights & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white_bishops & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white_rooks & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white_queens & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->white_kings & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"\n");
  }

  wprintf(L"\nBlack Pieces:\n");
  wprintf(L"Pawns     Knights   Bishops   Rooks     Queens    Kings\n");

  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black_pawns & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black_knights & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black_bishops & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black_rooks & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black_queens & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"  ");
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->black_kings & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"\n");
  }

  wprintf(L"\nCastling: %d\n", board->castle);
  wprintf(L"En passant: \n");
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      wprintf(L"%lc", (board->ep & (1ULL << square)) ? L'1' : L'.');
    }
    wprintf(L"\n");
  }
}

void board_list_moves(Board *board) {
  // Lists all the moves in the history stack
  Stack *history = board->history;
  for (int i = 0; i < history->last_move; i++) {
    Move move = history->list_of_move[i];
    wprintf(L"%d: %d %d -> %d %d ( %lc )\n", i, move.from & 7, move.from >> 3,
            move.to & 7, move.to >> 3, piece_chars[move.piece]);
  }
}
