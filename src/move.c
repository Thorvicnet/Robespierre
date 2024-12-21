#include "move.h"
#include "board.h"

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
  default:
    return false;
  }
}
