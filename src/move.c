#include "move.h"

// TODO: Check Discovered Check

// Each pieces check if the destination is allowed and if they moved correctly

static inline int min(int a, int b) { return a > b ? b : a; }
static inline int max(int a, int b) { return a > b ? a : b; }

bool check_piece_color(Board *board, int pos) {
  int piece = board_get(board, pos);
  if ((piece != EMPTY) &&
      ((piece & 0xF0) ==
       board->color)) { // You are trying to eat your own piece
    return false;
  }
  return true;
}

bool check_knight(Board *board, int orig[2], int dest[2]) {
  return check_piece_color(board, dest[0] + dest[1] * 8) &&
         ((abs(orig[0] - dest[0]) == 2 && abs(orig[1] - dest[1]) == 1) ||
          (abs(orig[0] - dest[0]) == 1 && abs(orig[1] - dest[1]) == 2));
}

// Rook: Validate vertical or horizontal moves and check for obstructions
bool check_rook(Board *board, int orig[2], int dest[2]) {
  if (orig[0] == dest[0]) { // Vertical
    int col = orig[0];
    int start = min(orig[1], dest[1]) + 1;
    int end = max(orig[1], dest[1]);
    for (int row = start; row < end; row++) {
      if (board_get(board, col + row * 8) != EMPTY) {
        return false;
      }
    }
  } else if (orig[1] == dest[1]) { // Horizontal
    int row = orig[1];
    int start = min(orig[0], dest[0]) + 1;
    int end = max(orig[0], dest[0]);
    for (int col = start; col < end; col++) {
      if (board_get(board, col + row * 8) != EMPTY) {
        return false;
      }
    }
  } else {
    return false;
  }
  return check_piece_color(board, dest[0] + dest[1] * 8);
}

// Bishop: Validate diagonal moves and check for obstructions
bool check_bishop(Board *board, int orig[2], int dest[2]) {
  if (abs(orig[0] - dest[0]) != abs(orig[1] - dest[1])) {
    return false; // Not a diagonal move
  }

  int row_step = (dest[1] > orig[1]) ? 1 : -1; // Direction for rows
  int col_step = (dest[0] > orig[0]) ? 1 : -1; // Direction for columns

  int row = orig[1] + row_step;
  int col = orig[0] + col_step;

  while (row != dest[1] && col != dest[0]) {
    if (board_get(board, col + row * 8) != EMPTY) {
      return false;
    }
    row += row_step;
    col += col_step;
  }

  return check_piece_color(board, dest[0] + dest[1] * 8);
}

bool check_queen(Board *board, int orig[2], int dest[2]) {
  return check_bishop(board, orig, dest) || check_rook(board, orig, dest);
}

bool can_enpassant(Board *board, int orig_pos, int dest_pos) {
  if (board->history->last_move == 0)
    return false;

  Move last_move = board_last_move(board);
  int orig_file = orig_pos % 8;
  int orig_rank = orig_pos / 8;
  int dest_file = dest_pos % 8;

  if ((last_move.piece & 0x0F) != PAWN)
    return false;
  if (abs(last_move.orig[1] - last_move.dest[1]) != 2)
    return false;

  int correct_rank = ((board->color == WHITE) ? 4 : 3);
  if (orig_rank != correct_rank)
    return false;

  if (dest_file != last_move.dest[0])
    return false;
  if (abs(orig_file - last_move.dest[0]) != 1)
    return false;

  return true;
}

bool check_pawn(Board *board, int orig[2], int dest[2]) {
  int orig_pos = orig[0] + orig[1] * 8;
  int dest_pos = dest[0] + dest[1] * 8;

  int piece = board_get(board, orig_pos);
  int color = piece & 0xF0;
  int direction = (color == WHITE) ? 1 : -1;

  int file_diff = dest[0] - orig[0];
  int rank_diff = dest[1] - orig[1];

  // Forward
  if (file_diff == 0) {
    // 1 square
    if (rank_diff == direction && board_get(board, dest_pos) == EMPTY) {
      return true;
    }
    // 2 squares
    if (orig[1] == (color == WHITE ? 1 : 6) && rank_diff == 2 * direction &&
        board_get(board, dest_pos) == EMPTY &&
        board_get(board, orig[0] + (orig[1] + direction) * 8) == EMPTY) {
      return true;
    }
    return false;
  }

  // captures
  if (abs(file_diff) == 1 && rank_diff == direction) {
    // normal
    int dest_piece = board_get(board, dest_pos);
    if (dest_piece != EMPTY && (dest_piece & 0xF0) != color) {
      return true;
    }
    // en passant
    if (can_enpassant(board, orig_pos, dest_pos)) {
      return true;
    }
  }

  return false;
}

bool king_moved(Board *board, int color) {
  // Returns true if the king of the given color has moved
  for (int i = 0; i < board->history->last_move; i++) {
    int piece = board->history->list_of_move[i].piece;
    if ((piece & 0x0F) == KING && (piece & 0xF0) == color) {
      return true;
    }
  }
  return false;
}

bool rook_moved(Board *board, int color, int orig[2]) {
  // Returns true if the king of the given color has moved
  for (int i = 0; i < board->history->last_move; i++) {
    Move move = board->history->list_of_move[i];
    int piece = move.piece;
    if ((piece & 0x0F) == ROOK && (piece & 0xF0) == color &&
        move.orig[0] == orig[0] && move.orig[1] == orig[1]) {
      return true;
    }
  }
  return false;
}

bool check_king(Board *board, int orig[2], int dest[2]) {
  if (abs(orig[0] - dest[0]) <= 1 && abs(orig[1] - dest[1]) <= 1 &&
      (abs(orig[0] - dest[0]) != 0 || abs(orig[1] - dest[1]) != 0)) {
    return check_piece_color(board, dest[0] + dest[1] * 8);
  }

  // Castling
  if (orig[1] != dest[1] || (dest[0] != 2 && dest[0] != 6) ||
      abs(orig[0] - dest[0]) != 2) {
    return false;
  }

  // Set rook positions
  int rook_orig[2], rook_dest[2];
  if (dest[0] == 2) { // Queen-side
    rook_orig[0] = 0;
    rook_orig[1] = orig[1];
    rook_dest[0] = 3;
    rook_dest[1] = orig[1];
  } else { // King-side
    rook_orig[0] = 7;
    rook_orig[1] = orig[1];
    rook_dest[0] = 5;
    rook_dest[1] = orig[1];
  }

  // Check if path is clear between king and rook
  int start = min(orig[0], rook_orig[0]);
  int end = max(orig[0], rook_orig[0]);
  for (int col = start + 1; col < end; col++) {
    if (board_get(board, col + orig[1] * 8) != EMPTY) {
      return false;
    }
  }

  // Verify rook presence and color
  int rook_piece = board_get(board, rook_orig[0] + rook_orig[1] * 8);
  if ((rook_piece & 0x0F) != ROOK || (rook_piece & 0xF0) != board->color) {
    return false;
  }

  // Check if pieces haven't moved
  if (king_moved(board, board->color) ||
      rook_moved(board, board->color, rook_orig)) {
    return false;
  }

  return true;
}

bool move_check_validity(Board *board, int orig[2], int dest[2]) {
  if (dest[0] >= 8 || dest[1] >= 8 || dest[0] < 0 || dest[1] < 0) {
    return false;
  }
  int piece = board_get(board, orig[0] + orig[1] * 8);
  if ((piece & 0xF0) != board->color) {
    return false;
  }
  switch (piece & 0x0F) {
  case BISHOP:
    return check_bishop(board, orig, dest);
  case ROOK:
    return check_rook(board, orig, dest);
  case KNIGHT:
    return check_knight(board, orig, dest);
  case QUEEN:
    return check_queen(board, orig, dest);
  case PAWN:
    return check_pawn(board, orig, dest);
  case KING:
    return check_king(board, orig, dest);
  default:
    return false;
  }
}

void move(Board *board, Move move) {
  if (!move_check_validity(board, move.orig, move.dest)) {
    return;
  }

  int orig_pos = move.orig[0] + move.orig[1] * 8;
  int dest_pos = move.dest[0] + move.dest[1] * 8;
  int piece = move.piece;

  if ((piece & 0x0F) == PAWN && board_get(board, dest_pos) == EMPTY &&
      move.orig[0] != move.dest[0]) {
    int captured_rank = move.orig[1];
    board_set(board, move.dest[0] + captured_rank * 8, EMPTY);
  } else if ((piece & 0x0F) == KING && abs(move.orig[0] - move.dest[0]) == 2) {
    int rook_orig[2], rook_dest[2];
    if (move.dest[0] == 2) { // Queen-side castling
      rook_orig[0] = 0;
      rook_orig[1] = move.orig[1];
      rook_dest[0] = 3;
      rook_dest[1] = move.orig[1];
    } else if (move.dest[0] == 6) { // King-side castling
      rook_orig[0] = 7;
      rook_orig[1] = move.orig[1];
      rook_dest[0] = 5;
      rook_dest[1] = move.orig[1];
    }

    board_set(board, rook_dest[0] + rook_dest[1] * 8,
              board_get(board, rook_orig[0] + rook_orig[1] * 8));
    board_set(board, rook_orig[0] + rook_orig[1] * 8, EMPTY);
  }
  board_set(board, dest_pos, piece);
  board_set(board, orig_pos, EMPTY);

  if ((piece & 0x0F) == PAWN) {
    if ((piece & 0xF0) == WHITE && move.dest[1] == 7) {
      board_set(board, dest_pos, WHITE_QUEEN);
    } else if ((piece & 0xF0) == BLACK && move.dest[1] == 0) {
      board_set(board, dest_pos, BLACK_QUEEN);
    }
  }

  board_add_move(board, move);

  board->color ^= BLACK;
}
