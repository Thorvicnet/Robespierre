#include "move.h"
#include "board.h"
#include "types.h"
#include <time.h>

// TODO: Check Discovered Check

// Each pieces check if the destination is allowed and if they moved correctly

static inline int min(int a, int b) { return a > b ? b : a; }
static inline int max(int a, int b) { return a > b ? a : b; }

bool check_piece_color(Board *board, int pos) {
  int piece = board_get(board, pos);
  if ((piece != EMPTY) &&
      (COLOR(piece) == board->color)) { // You are trying to eat your own piece
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
  Bb valid = bb_rook_attacks(board->all, orig[0] + orig[1] * 8) &
             ~(board->color == WHITE ? board->white : board->black);
  return IS_BIT_SET(valid, dest[0] + dest[1] * 8);
}

// Bishop: Validate diagonal moves and check for obstructions
bool check_bishop(Board *board, int orig[2], int dest[2]) {
  Bb valid = bb_bishop_attacks(board->all, orig[0] + orig[1] * 8) &
             ~(board->color == WHITE ? board->white : board->black);
  return IS_BIT_SET(valid, dest[0] + dest[1] * 8);
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

  if (PIECE(last_move.piece) != PAWN)
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
  int color = COLOR(piece);
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
    if (dest_piece != EMPTY && COLOR(dest_piece) != color) {
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
    if (PIECE(piece) == KING && COLOR(piece) == color) {
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
    if (PIECE(piece) == ROOK && COLOR(piece) == color &&
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
  if (PIECE(rook_piece) != ROOK || COLOR(rook_piece) != board->color) {
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
  if (COLOR(piece) != board->color) {
    return false;
  }
  switch (PIECE(piece)) {
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

  if (PIECE(piece) == PAWN && board_get(board, dest_pos) == EMPTY &&
      move.orig[0] != move.dest[0]) {
    int captured_rank = move.orig[1];
    board_set(board, move.dest[0] + captured_rank * 8, EMPTY);
  } else if (PIECE(piece) == KING && abs(move.orig[0] - move.dest[0]) == 2) {
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

  if (PIECE(piece) == PAWN) {
    if (COLOR(piece) == WHITE && move.dest[1] == 7) {
      board_set(board, dest_pos, WHITE_QUEEN);
    } else if (COLOR(piece) == BLACK && move.dest[1] == 0) {
      board_set(board, dest_pos, BLACK_QUEEN);
    }
  }

  board_add_move(board, move);

  board->color ^= BLACK;
}

List_of_move knight_possible_move(Board *board, int pos[2]) {
  int offset[8][2] = {{2, 1}, {2, -1}, {-2, -1}, {-2, 1},
                      {1, 2}, {1, -2}, {-1, 2},  {-1, -2}};
  Move *result_move = (Move *)malloc(sizeof(Move) * 8);
  int nb = 0;
  for (int i = 0; i < 8; i++) {
    int dest[2] = {pos[0] + offset[i][0], pos[1] + offset[i][1]};
    if (dest[0] >= 0 && dest[0] < 8 && dest[1] >= 0 && dest[1] < 8 &&
        check_piece_color(board, dest[0] + 8 * dest[1])) {
      Move move = {.piece = board_get(board, pos[0] + 8 * dest[0]),
                   {pos[0], pos[1]},
                   {dest[0], dest[1]},
                   board_get(board, dest[0] + 8 * dest[1]) != EMPTY};
      result_move[nb] = move;
      nb++;
    }
  }
  List_of_move list = {result_move, nb};
  return list;
}

List_of_move rook_possible_move(Board *board, int pos[2]) {
  Move *result_move = (Move *)malloc(sizeof(Move) * 16);
  int nb = 0;
  Bb valid = bb_rook_attacks(board->all, pos[0] + pos[1] * 8) &
             ~(board->color == WHITE ? board->white : board->black);
  for (int i = 0; i < 8; i++) {
    int dest[2] = {pos[0] + i, pos[1]};
    if (IS_BIT_SET(valid, dest[0] + dest[1] * 8)) {
      Move move = {.piece = board_get(board, pos[0] + 8 * dest[0]),
                   {pos[0], pos[1]},
                   {dest[0], dest[1]},
                   board_get(board, dest[0] + 8 * dest[1]) != EMPTY};
      result_move[nb] = move;
      nb++;
    } else {
      break;
    }
  }
  List_of_move list = {result_move, nb};
  return list;
}

List_of_move bishop_possible_move(Board *board, int pos[2]) {
  Move *result_move = (Move *)malloc(sizeof(Move) * 16);
  int nb = 0;
  Bb valid = bb_bishop_attacks(board->all, pos[0] + pos[1] * 8) &
             ~(board->color == WHITE ? board->white : board->black);
  for (int i = 0; i < 8; i++) {
    int dest[2] = {pos[0] + i, pos[1] + i};
    if (IS_BIT_SET(valid, dest[0] + dest[1] * 8)) {
      Move move = {.piece = board_get(board, pos[0] + 8 * dest[0]),
                   {pos[0], pos[1]},
                   {dest[0], dest[1]},
                   board_get(board, dest[0] + 8 * dest[1]) != EMPTY};
      result_move[nb] = move;
      nb++;
    } else {
      break;
    }
  }
  List_of_move list = {result_move, nb};
  return list;
}

List_of_move merge_list_of_move(List_of_move list1, List_of_move list2) {
  Move *list = (Move *)malloc(sizeof(Move) * (list1.nb + list2.nb));
  for (int i = 0; i < list1.nb; i++) {
    list[i] = list1.list[i];
  }
  for (int i = 0; i < list2.nb; i++) {
    list[i + list1.nb] = list2.list[i];
  }
  List_of_move ret = {list, list1.nb + list2.nb};
  free(list1.list);
  free(list2.list);
  return ret;
}

List_of_move pawn_possible_move(Board *board, int pos[2]) {
  int offset[8][2] = {{0, 1},  {0, 2},  {1, 1},   {1, -1},
                      {0, -1}, {0, -2}, {-1, -1}, {-1, 1}};
  Move *result_move = (Move *)malloc(sizeof(Move) * 4);
  int nb = 0;
  for (int i = 0; i < 8; i++) {
    int dest[2] = {pos[0] + offset[i][0], pos[1] + offset[i][1]};
    if (check_pawn(board, pos, dest)) {
      Move move = {.piece = board_get(board, pos[0] + 8 * dest[0]),
                   {pos[0], pos[1]},
                   {dest[0], dest[1]},
                   board_get(board, dest[0] + 8 * dest[1]) != EMPTY};
      result_move[nb] = move;
      nb++;
    }
  }
  List_of_move list = {result_move, nb};
  return list;
}

List_of_move queen_possible_move(Board *board, int pos[2]) {
  return merge_list_of_move(rook_possible_move(board, pos),
                            bishop_possible_move(board, pos));
}

List_of_move any_possible_move(Board *board, int pos[2], int piece) {

  switch (piece & 0x0F0) {
  case QUEEN:
    return queen_possible_move(board, pos);
  case ROOK:
    return rook_possible_move(board, pos);
  case BISHOP:
    return bishop_possible_move(board, pos);
  case KNIGHT:
    return knight_possible_move(board, pos);
  case PAWN:
    return pawn_possible_move(board, pos);
  }
  return (List_of_move){NULL, 0};
}

List_of_move possible_move(Board *board) {
  List_of_move ret = {NULL, 0};
  for (int i = 0; i < 64; i++) {
    int piece = board_get(board, i);
    if (piece != EMPTY && ((piece & 0xF0) == board->color)) {
      int pos[2] = {i - i / 8, i / 8};
      merge_list_of_move(ret, any_possible_move(board, pos, piece));
    }
  }
  return ret;
}
