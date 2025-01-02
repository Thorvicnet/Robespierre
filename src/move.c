#include "move.h"
#include "board.h"
#include "threat.h"
#include "types.h"
#include <time.h>

// Each piece checks whether the destination is allowed and whether it has moved
// correctly

bool check_knight(Board *board, int orig[2], int dest[2]) {
#ifdef MENACE
  return IS_BIT_SET(~(board->color == WHITE
                          ? board->white & ~(board->black_threat)
                          : board->black & ~(board->white_threat)),
                    dest[0] + dest[1] * 8) &&
         ((abs(orig[0] - dest[0]) == 2 && abs(orig[1] - dest[1]) == 1) ||
          (abs(orig[0] - dest[0]) == 1 && abs(orig[1] - dest[1]) == 2));
#else
  return IS_BIT_SET(~(board->color == WHITE ? board->white : board->black),
                    dest[0] + dest[1] * 8) &&
         ((abs(orig[0] - dest[0]) == 2 && abs(orig[1] - dest[1]) == 1) ||
          (abs(orig[0] - dest[0]) == 1 && abs(orig[1] - dest[1]) == 2));
#endif
}

// Rook: Validate vertical or horizontal moves and check for obstructions
bool check_rook(Board *board, int orig[2], int dest[2]) {
#ifdef MENACE
  Bb valid = bb_rook_attacks(board->all, orig[0] + orig[1] * 8) &
             ~(board->color == WHITE ? board->white & ~(board->black_threat)
                                     : board->black & ~(board->white_threat));
#else
  Bb valid = bb_rook_attacks(board->all, orig[0] + orig[1] * 8) &
             ~(board->color == WHITE ? board->white : board->black);
#endif
  return IS_BIT_SET(valid, dest[0] + dest[1] * 8);
}

// Bishop: Validate diagonal moves and check for obstructions
bool check_bishop(Board *board, int orig[2], int dest[2]) {
#ifdef MENACE
  Bb valid = bb_bishop_attacks(board->all, orig[0] + orig[1] * 8) &
             ~(board->color == WHITE ? board->white & ~(board->black_threat)
                                     : board->black & ~(board->white_threat));
#else
  Bb valid = bb_bishop_attacks(board->all, orig[0] + orig[1] * 8) &
             ~(board->color == WHITE ? board->white : board->black);
#endif
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
  int direction = (COLOR(piece) == WHITE) ? 1 : -1;

  int file_diff = dest[0] - orig[0];
  int rank_diff = dest[1] - orig[1];

  // Forward
  if (file_diff == 0) {
    // 1 square
    if (rank_diff == direction && board_get(board, dest_pos) == EMPTY) {
      return true;
    }
    // 2 squares
    if (orig[1] == (COLOR(piece) == WHITE ? 1 : 6) &&
        rank_diff == 2 * direction && board_get(board, dest_pos) == EMPTY &&
        board_get(board, orig[0] + (orig[1] + direction) * 8) == EMPTY) {
      return true;
    }
    return false;
  }

  // captures
  if (abs(file_diff) == 1 && rank_diff == direction) {
    // normal
#ifdef MENACE
    if (IS_BIT_SET(board->color == WHITE ? board->black | board->black_threat
                                         : board->white | board->white_threat,
                   dest_pos)) {
      return true;
    }
#else
    int dest_piece = board_get(board, dest_pos);
    if (COLOR(dest_piece) != board->color && dest_piece != EMPTY) {
      return true;
    }
#endif
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
  int orig_sq = orig[0] + orig[1] * 8;
  int dest_sq = dest[0] + dest[1] * 8;

  if (IS_BIT_SET(KING_MASKS[orig_sq], dest_sq)) {
    Bb friendly_pieces = (board->color == WHITE) ? board->white : board->black;
    return !IS_BIT_SET(friendly_pieces, dest_sq);
  }

  // Castling
  if (abs(orig[0] - dest[0]) == 2 && orig[1] == dest[1]) {
    if (orig[1] != (board->color == WHITE ? 0 : 7) || orig[0] != 3)
      return false;

    if (king_moved(board, board->color))
      return false;

    bool kingside = (dest[0] == 1);
    int rook_file = kingside ? 0 : 7;
    int rook_rank = board->color == WHITE ? 0 : 7;
    int rook_sq = rook_file + rook_rank * 8;

    int rook_piece = board_get(board, rook_sq);
    if (PIECE(rook_piece) != ROOK || COLOR(rook_piece) != board->color ||
        rook_moved(board, board->color, (int[]){rook_file, rook_rank}))
      return false;

    Bb path = 0ULL;
    if (kingside) {
      path = (1ULL << (orig_sq - 1)) | (1ULL << (orig_sq - 2));
    } else {
      path = (1ULL << (orig_sq + 1)) | (1ULL << (orig_sq + 2)) |
             (1ULL << (orig_sq + 3));
    }
    if (board->all & path)
      return false;

    Bb threats = threat_board_squares(board, !board->color);
    Bb king_squares = (1ULL << orig_sq) | path;

    if (threats & king_squares)
      return false;
    return true;
  }

  return false;
}

bool move_check_validity(Board *board, int orig[2], int dest[2]) {
  if (dest[0] >= 8 || dest[1] >= 8 || dest[0] < 0 || dest[1] < 0) {
    return false;
  }
  int piece = board_get(board, orig[0] + orig[1] * 8);
#ifdef MENACE
  // You can play if you threaten or possess the piece
  if (!IS_BIT_SET(board->color == WHITE ? board->white | board->white_threat
                                        : board->black | board->black_threat,
                  orig[0] + orig[1] * 8)) {
    return false;
  }
#else
  if (COLOR(piece) != board->color) {
    return false;
  }
#endif
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

void move(Board *orig_board, Move move) {
  Board *board = board_copy(
      orig_board); // TODO: there HAS to be a better way but ATM it works
  if (!move_check_validity(board, move.orig, move.dest)) {
    return;
  }

  int orig_pos = move.orig[0] + move.orig[1] * 8;
  int dest_pos = move.dest[0] + move.dest[1] * 8;
  int piece = move.piece;

  // En passant and castling
  if (PIECE(piece) == PAWN && board_get(board, dest_pos) == EMPTY &&
      move.orig[0] != move.dest[0]) {
    int captured_rank = move.orig[1];
    board_set(board, move.dest[0] + captured_rank * 8, EMPTY);
  } else if (PIECE(piece) == KING && abs(move.orig[0] - move.dest[0]) == 2) {
    int rook_orig[2], rook_dest[2];
    if (move.dest[0] == 5) { // Queen-side castling
      rook_orig[0] = 7;
      rook_orig[1] = move.orig[1];
      rook_dest[0] = 4;
      rook_dest[1] = move.orig[1];
    } else if (move.dest[0] == 1) { // King-side castling
      rook_orig[0] = 0;
      rook_orig[1] = move.orig[1];
      rook_dest[0] = 2;
      rook_dest[1] = move.orig[1];
    }
    board_set(board, rook_dest[0] + rook_dest[1] * 8,
              board_get(board, rook_orig[0] + rook_orig[1] * 8));
    board_set(board, rook_orig[0] + rook_orig[1] * 8, EMPTY);
  }
  board_set(board, dest_pos, piece);
  board_set(board, orig_pos, EMPTY);

  // Promotion
  if (PIECE(piece) == PAWN) {
    if (COLOR(piece) == WHITE && move.dest[1] == 7) {
      board_set(board, dest_pos, WHITE_QUEEN);
    } else if (COLOR(piece) == BLACK && move.dest[1] == 0) {
      board_set(board, dest_pos, BLACK_QUEEN);
    }
  }

  board->color ^= BLACK;

  threat_board_update(board);
  if (threat_check(board))
    return;

  board_add_move(orig_board, move);
  *orig_board = *board; // this is plain disgusting
}

// Beginning move enumeration //

void init_move_list(MoveList *list) {
  list->count = 0;
  list->capacity = 16;
  list->moves = (Move *)malloc(sizeof(Move) * list->capacity);
  if (list->moves == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }
}

void add_move(MoveList *list, Move move) {
  if (list->count == list->capacity) {
    list->capacity *= 2;
    list->moves = (Move *)realloc(list->moves, sizeof(Move) * list->capacity);
  }
  list->moves[list->count++] = move;
}

void free_move_list(MoveList *list) {
  free(list->moves);
  list->moves = NULL;
  list->count = 0;
  list->capacity = 0;
}

void knight_possible_move(Board *board, int pos[2], MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = KNIGHT_MASKS[pos[0] + pos[1] * 8] &
          ~(board->color == WHITE ? board->white & ~board->black_threat
                                  : board->black & ~board->white_threat);
#else
  valid = KNIGHT_MASKS[pos[0] + pos[1] * 8] &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);
    int dest_x = dest_sq & 7;
    int dest_y = dest_sq >> 3;

    Move move = {board_get(board, pos[0] + 8 * pos[1]),
                 {pos[0], pos[1]},
                 {dest_x, dest_y},
                 board_get(board, dest_x + 8 * dest_y) != EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void rook_possible_move(Board *board, int pos[2], MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = bb_rook_attacks(board->all, pos[0] + pos[1] * 8) &
          ~(board->color == WHITE ? board->white & ~board->black_threat
                                  : board->black & ~board->white_threat);
#else
  valid = bb_rook_attacks(board->all, pos[0] + pos[1] * 8) &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);
    int dest_x = dest_sq & 7;
    int dest_y = dest_sq >> 3;

    Move move = {board_get(board, pos[0] + 8 * pos[1]),
                 {pos[0], pos[1]},
                 {dest_x, dest_y},
                 board_get(board, dest_x + 8 * dest_y) != EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void bishop_possible_move(Board *board, int pos[2], MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = bb_bishop_attacks(board->all, pos[0] + pos[1] * 8) &
          ~(board->color == WHITE ? board->white & ~board->black_threat
                                  : board->black & ~board->white_threat);
#else
  valid = bb_bishop_attacks(board->all, pos[0] + pos[1] * 8) &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);
    int dest_x = dest_sq & 7;
    int dest_y = dest_sq >> 3;

    Move move = {board_get(board, pos[0] + 8 * pos[1]),
                 {pos[0], pos[1]},
                 {dest_x, dest_y},
                 board_get(board, dest_x + 8 * dest_y) != EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void pawn_possible_move(Board *board, int pos[2], MoveList *list) {
  int offset[8][2] = {
      {0, 1},   {0, 2},
      {1, 1},   {1, -1}, // TODO: you can reduce the search knowing the color
      {0, -1},  {0, -2},
      {-1, -1}, {-1, 1}};
  for (int i = 0; i < 8; i++) {
    int dest[2] = {pos[0] + offset[i][0], pos[1] + offset[i][1]};
    if (check_pawn(board, pos, dest)) {
      Move move = {board_get(board, pos[0] + 8 * pos[1]),
                   {pos[0], pos[1]},
                   {dest[0], dest[1]},
                   board_get(board, dest[0] + 8 * dest[1]) != EMPTY};
      add_move(list, move);
    }
  }
}

void queen_possible_move(Board *board, int pos[2], MoveList *list) {
  rook_possible_move(board, pos, list);
  bishop_possible_move(board, pos, list);
}

void any_possible_move(Board *board, int pos[2], int piece, MoveList *list) {
  switch (piece & 0x0F) {
  case QUEEN:
    queen_possible_move(board, pos, list);
    break;
  case ROOK:
    rook_possible_move(board, pos, list);
    break;
  case BISHOP:
    bishop_possible_move(board, pos, list);
    break;
  case KNIGHT:
    knight_possible_move(board, pos, list);
    break;
  case PAWN:
    pawn_possible_move(board, pos, list);
    break;
  }
}

MoveList possible_move(Board *board) {
  MoveList ret;
  init_move_list(&ret);
  for (int i = 0; i < 64; i++) {
    int piece = board_get(board, i);
    if (piece != EMPTY && ((piece & 0xF0) == board->color)) {
      int pos[2] = {i % 8, i / 8};
      any_possible_move(board, pos, piece, &ret);
    }
  }
  return ret;
}
