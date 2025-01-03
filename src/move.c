#include "move.h"

// Each piece checks whether the destination is allowed and whether it has moved
// correctly

bool check_knight(Board *board, int orig, int dest) {
#ifdef MENACE
  return IS_BIT_SET(~(board->color == WHITE
                          ? board->white & ~(board->black_threat)
                          : board->black & ~(board->white_threat)),
                    dest) &&
         ((abs((orig & 7) - (dest & 7)) == 2 && abs((orig >> 3) - (dest >> 3)) == 1) ||
          (abs((orig & 7) - (dest & 7)) == 1 && abs((orig >> 3) - (dest >> 3)) == 2));
#else
  return IS_BIT_SET(~(board->color == WHITE ? board->white : board->black),
                    dest) &&
         ((abs((orig & 7) - (dest & 7)) == 2 && abs((orig >> 3) - (dest >> 3)) == 1) ||
          (abs((orig & 7) - (dest & 7)) == 1 && abs((orig >> 3) - (dest >> 3)) == 2));
#endif
}

// Rook: Validate vertical or horizontal moves and check for obstructions
bool check_rook(Board *board, int orig, int dest) {
#ifdef MENACE
  Bb valid = bb_rook_attacks(board->all, orig) &
             ~(board->color == WHITE ? board->white & ~(board->black_threat)
                                     : board->black & ~(board->white_threat));
#else
  Bb valid = bb_rook_attacks(board->all, orig) &
             ~(board->color == WHITE ? board->white : board->black);
#endif
  return IS_BIT_SET(valid, dest);
}

// Bishop: Validate diagonal moves and check for obstructions
bool check_bishop(Board *board, int orig, int dest) {
#ifdef MENACE
  Bb valid = bb_bishop_attacks(board->all, orig) &
             ~(board->color == WHITE ? board->white & ~(board->black_threat)
                                     : board->black & ~(board->white_threat));
#else
  Bb valid = bb_bishop_attacks(board->all, orig) &
             ~(board->color == WHITE ? board->white : board->black);
#endif
  return IS_BIT_SET(valid, dest);
}

bool check_queen(Board *board, int orig, int dest) {
  return check_bishop(board, orig, dest) || check_rook(board, orig, dest);
}

bool can_enpassant(Board *board, int orig_pos, int dest_pos) {
  if (board->history->last_move == 0)
    return false;

  Move last_move = board_last_move(board);
  int orig_file = orig_pos & 7;
  int orig_rank = orig_pos >> 3;
  int dest_file = dest_pos & 7;

  if (PIECE(last_move.piece) != PAWN)
    return false;
  if (abs((last_move.from >> 3) - (last_move.to >> 3)) != 2)
    return false;

  int correct_rank = ((board->color == WHITE) ? 4 : 3);
  if (orig_rank != correct_rank)
    return false;

  if (dest_file != (last_move.to & 7))
    return false;
  if (abs(orig_file - (last_move.to & 7)) != 1)
    return false;

  return true;
}

bool check_pawn(Board *board, int orig, int dest) {
  int piece = board_get(board, orig);
  int direction = (COLOR(piece) == WHITE) ? 1 : -1;

  int file_diff = (dest & 7) - (orig & 7);
  int rank_diff = (dest >> 3) - (orig >> 3);

  // Forward
  if (file_diff == 0) {
    // 1 square
    if (rank_diff == direction && board_get(board, dest) == EMPTY) {
      return true;
    }
    // 2 squares
    if ((orig >> 3) == (COLOR(piece) == WHITE ? 1 : 6) &&
        rank_diff == 2 * direction && board_get(board, dest) == EMPTY &&
        board_get(board, (orig & 7) + ((orig >> 3) + direction) * 8) == EMPTY) {
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
                   dest)) {
      return true;
    }
#else
    int dest_piece = board_get(board, dest);
    if (COLOR(dest_piece) != board->color && dest_piece != EMPTY) {
      return true;
    }
#endif
    // en passant
    if (can_enpassant(board, orig, dest)) {
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

bool rook_moved(Board *board, int color, int orig) {
  // Returns true if the king of the given color has moved
  for (int i = 0; i < board->history->last_move; i++) {
    Move move = board->history->list_of_move[i];
    int piece = move.piece;
    if (PIECE(piece) == ROOK && COLOR(piece) == color &&
        move.from == orig) {
      return true;
    }
  }
  return false;
}

bool check_king(Board *board, int orig, int dest) {
  if (IS_BIT_SET(KING_MASKS[orig], dest)) {
    Bb friendly_pieces = (board->color == WHITE) ? board->white : board->black;
    return !IS_BIT_SET(friendly_pieces, dest);
  }

  // Castling
  if (abs((orig & 7) - (dest & 7)) == 2 && (orig >> 3) == (dest >> 3)) {
    if ((orig >> 3) != (board->color == WHITE ? 0 : 7) || (orig & 7) != 3)
      return false;

    if (king_moved(board, board->color))
      return false;

    bool kingside = ((dest & 7) == 1);
    int rook_file = kingside ? 0 : 7;
    int rook_rank = board->color == WHITE ? 0 : 7;
    int rook_sq = rook_file + rook_rank * 8;

    int rook_piece = board_get(board, rook_sq);
    if (PIECE(rook_piece) != ROOK || COLOR(rook_piece) != board->color ||
        rook_moved(board, board->color, rook_sq))
      return false;

    Bb path = 0ULL;
    if (kingside) {
      path = (1ULL << (orig - 1)) | (1ULL << (orig - 2));
    } else {
      path = (1ULL << (orig + 1)) | (1ULL << (orig + 2)) |
             (1ULL << (orig + 3));
    }
    if (board->all & path)
      return false;

    Bb threats = threat_board_squares(board, !board->color);
    Bb king_squares = (1ULL << orig) | path;

    if (threats & king_squares)
      return false;
    return true;
  }

  return false;
}

bool move_check_validity(Board *board, int orig, int dest) {
  if (dest < 0 || dest >= 64) {
    return false;
  }
  int piece = board_get(board, orig);
#ifdef MENACE
  // You can play if you threaten or possess the piece
  if (!IS_BIT_SET(board->color == WHITE ? board->white | board->white_threat
                                        : board->black | board->black_threat,
                  orig)) {
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

int move(Board *orig_board, Move move) {
  Board *board = board_copy(
      orig_board); // TODO: there HAS to be a better way but ATM it works
  if (!move_check_validity(board, move.from, move.to)) {
    board_free(board);
    return -1;
  }

  int orig_pos = move.from;
  int dest_pos = move.to;
  int piece = move.piece;

  // En passant and castling
  if (PIECE(piece) == PAWN && board_get(board, dest_pos) == EMPTY &&
      (orig_pos & 7) != (dest_pos & 7)) {
    int captured_rank = orig_pos >> 3;
    board_set(board, (dest_pos & 7) + captured_rank * 8, EMPTY);
  } else if (PIECE(piece) == KING && abs((orig_pos & 7) - (dest_pos & 7)) == 2) {
    int rook_orig, rook_dest;
    if ((dest_pos & 7) == 5) { // Queen-side castling
      rook_orig = 7 + (orig_pos >> 3) * 8;
      rook_dest = 4 + (orig_pos >> 3) * 8;
    } else if ((dest_pos & 7) == 1) { // King-side castling
      rook_orig = 0 + (orig_pos >> 3) * 8;
      rook_dest = 2 + (orig_pos >> 3) * 8;
    }
    board_set(board, rook_dest,
              board_get(board, rook_orig));
    board_set(board, rook_orig, EMPTY);
  }
  board_set(board, dest_pos, piece);
  board_set(board, orig_pos, EMPTY);

  // Promotion
  if (PIECE(piece) == PAWN) {
    if (COLOR(piece) == WHITE && (dest_pos >> 3) == 7) {
      board_set(board, dest_pos, WHITE_QUEEN);
    } else if (COLOR(piece) == BLACK && (dest_pos >> 3) == 0) {
      board_set(board, dest_pos, BLACK_QUEEN);
    }
  }

  board->color ^= BLACK;

  threat_board_update(board);
  if (threat_check(board)) {
    board_free(board);
    return -1;
  }
  // board_add_move(orig_board, move); // FIXME: REMOVE THIS (just for testing depth)
  *orig_board = *board; // this is plain disgusting
  board_free(board);
  return 0;
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

void move_list_free(MoveList *list) {
  free(list->moves);
  list->moves = NULL;
  list->count = 0;
  list->capacity = 0;
}

void knight_possible_move(Board *board, int pos, MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = KNIGHT_MASKS[pos] &
          ~(board->color == WHITE ? board->white & ~board->black_threat
                                  : board->black & ~board->white_threat);
#else
  valid = KNIGHT_MASKS[pos] &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos),
                 pos,
                 dest_sq,
                 board_get(board, dest_sq) != EMPTY};

    add_move(list, move);
    valid &= valid - 1;
  }
}

void rook_possible_move(Board *board, int pos, MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = bb_rook_attacks(board->all, pos) &
          ~(board->color == WHITE ? board->white & ~board->black_threat
                                  : board->black & ~board->white_threat);
#else
  valid = bb_rook_attacks(board->all, pos) &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos),
                 pos,
                 dest_sq,
                 board_get(board, dest_sq) != EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void bishop_possible_move(Board *board, int pos, MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = bb_bishop_attacks(board->all, pos) &
          ~(board->color == WHITE ? board->white & ~board->black_threat
                                  : board->black & ~board->white_threat);
#else
  valid = bb_bishop_attacks(board->all, pos) &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos),
                 pos,
                 dest_sq,
                 board_get(board, dest_sq) != EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void pawn_possible_move(Board *board, int pos, MoveList *list) {
  int offset[8] = {8, 16, 9, 7, -8, -16, -9, -7};
  for (int i = 0; i < 8; i++) {
    int dest = pos + offset[i];
    if (dest < 0 || dest >= 64) // TODO: why does check_pawn not check this?
      continue;
    if (check_pawn(board, pos, dest)) {
      Move move = {board_get(board, pos),
                   pos,
                   dest,
                   board_get(board, dest) != EMPTY};
      add_move(list, move);
    }
  }
}

void queen_possible_move(Board *board, int pos, MoveList *list) {
  rook_possible_move(board, pos, list);
  bishop_possible_move(board, pos, list);
}

void king_possible_move(Board *board, int pos, MoveList *list) {
  #ifdef MENACE
  Bb valid = KING_MASKS[pos] &
             ~(board->color == WHITE ? board->white & ~board->black_threat
                                     : board->black & ~board->white_threat);
#else
  Bb valid = KING_MASKS[pos] &
             ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos),
                 pos,
                 dest_sq,
                 board_get(board, dest_sq) != EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void any_possible_move(Board *board, int pos, int piece, MoveList *list) {
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
  case KING:
    king_possible_move(board, pos, list);
    break;
  }
}

MoveList move_possible(Board *board) {
  MoveList ret;
  init_move_list(&ret);
  for (int i = 0; i < 64; i++) {
    int piece = board_get(board, i);
    if (piece != EMPTY && ((piece & 0xF0) == board->color)) {
      any_possible_move(board, i, piece, &ret);
    }
  }
  return ret;
}
