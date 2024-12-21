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

bool can_enpassant(Board *board, int pos) {
  Move last_move = board_last_move(board);
  if (last_move.piece != PAWN) {
    return false;
  }
  int color = board_get(board, pos) & 0xF0;

  if (abs(last_move.orig[1] - last_move.dest[1]) == 2) {
    if (last_move.dest[0] == pos % 8) {
            if ((color == WHITE && last_move.dest[1] == 4) ||
                (color == BLACK && last_move.dest[1] == 3)) {
                return true;
            }
    }
  }
  return false;
}

bool check_pawn(Board *board, int orig[2], int dest[2]) {
  int piece = board_get(board, orig[0] + orig[1] * 8);
  int color = piece & 0xF0;
  int piece_dest = board_get(board, dest[0] + dest[1] * 8);
  int direction = (color == WHITE) ? 1 : -1;
  int start_rank = (color == WHITE) ? 1 : 6;

  if (piece_dest == EMPTY) {
    // en passant
    if (abs(orig[0] - dest[0]) == 1 && (dest[1] - orig[1]) == direction) {
            if (can_enpassant(board, orig[0] + orig[1] * 8)) {
                return true;
            }
    }
    // one square
    if (orig[0] == dest[0] && (dest[1] - orig[1]) == direction) {
      return true;
    }
    // two squares
    if (orig[0] == dest[0] && orig[1] == start_rank &&
        (dest[1] - orig[1]) == 2 * direction) {
      int mid_square = orig[0] + (orig[1] + direction) * 8;
      int dest_square = dest[0] + dest[1] * 8;
      if (board_get(board, mid_square) == EMPTY && 
          board_get(board, dest_square) == EMPTY) {
        return true;
      }
    }
  }
  // capture
  else if ((piece_dest & 0xF0) != color) {
    return (abs(orig[0] - dest[0]) == 1 && (dest[1] - orig[1]) == direction);
  }
  return false;
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
  default:
    return false;
  }
}

void move(Board *board, Move move) {
  if (!move_check_validity(board, move.orig, move.dest)) {
    return;
  }
  board->moves++;
  board->color ^= BLACK;
  board_set(board, move.dest[0] + move.dest[1] * 8, move.piece);
  board_set(board, move.orig[0] + move.orig[1] * 8, EMPTY);
}
