#include "move.h"
#include "board.h"

// Each piece checks whether the destination is allowed and whether it has moved
// correctly

bool check_knight(Board *board, int orig, int dest) {
#ifdef MENACE
  return IS_BIT_SET(~(board->color == WHITE
                          ? board->white & ~(board->black_threat)
                          : board->black & ~(board->white_threat)),
                    dest) &&
         ((abs((orig & 7) - (dest & 7)) == 2 &&
           abs((orig >> 3) - (dest >> 3)) == 1) ||
          (abs((orig & 7) - (dest & 7)) == 1 &&
           abs((orig >> 3) - (dest >> 3)) == 2));
#else
  return IS_BIT_SET(~(board->color == WHITE ? board->white : board->black),
                    dest) &&
         ((abs((orig & 7) - (dest & 7)) == 2 &&
           abs((orig >> 3) - (dest >> 3)) == 1) ||
          (abs((orig & 7) - (dest & 7)) == 1 &&
           abs((orig >> 3) - (dest >> 3)) == 2));
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
             ~(board->color == WHITE ? board->white & ~board->black_threat
                                     : board->black & ~board->white_threat);
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
    if (IS_BIT_SET(board->color == WHITE
                       ? board->black | (board->black_threat & board->white &
                                         ~board->white_kings)
                       : board->white | (board->white_threat & board->black &
                                         ~board->black_kings),
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

bool check_king(Board *board, int orig, int dest) {
  if (IS_BIT_SET(KING_MASKS[orig], dest)) {
#ifdef MENACE
    Bb friendly_pieces = (board->color == WHITE)
                             ? board->white & ~board->black_threat
                             : board->black & ~board->white_threat;
#else
    Bb friendly_pieces = (board->color == WHITE) ? board->white : board->black;
#endif
    return !IS_BIT_SET(friendly_pieces, dest);
  }

  // Castling
  if (abs((orig & 7) - (dest & 7)) == 2 && (orig >> 3) == (dest >> 3)) {
    if ((orig >> 3) != (board->color == WHITE ? 0 : 7) || (orig & 7) != 3)
      return false;

    bool kingside = ((dest & 7) == 1);

    Bb path = 0ULL;
    if (kingside) {
      if (board->castle & (board->color == WHITE ? WHITE_CASTLE_KINGSIDE
                                                 : BLACK_CASTLE_KINGSIDE)) {
        return false;
      }
      path = (1ULL << (orig - 1)) | (1ULL << (orig - 2));
    } else {
      if (board->castle & (board->color == WHITE ? WHITE_CASTLE_QUEENSIDE
                                                 : BLACK_CASTLE_QUEENSIDE)) {
        return false;
      }
      path = (1ULL << (orig + 1)) | (1ULL << (orig + 2)) | (1ULL << (orig + 3));
    }
    if (board->all & path)
      return false;

    Bb king_squares = (1ULL << orig) | path;

    if ((board->color == WHITE ? board->black_threat : board->white_threat) &
        king_squares)
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
  if (!IS_BIT_SET(board->color == WHITE
                      ? board->white | (board->white_threat & board->black)
                      : board->black | (board->black_threat & board->white),
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

// Move a piece on the board
int move_make(Board *board, Move *move, Undo *undo) {
  int orig_pos = move->from;
  int dest_pos = move->to;
  int piece = move->piece;
  int capture = board_get(board, dest_pos);

  undo->castle = board->castle;
  undo->capture = capture;
  undo->ep = board->ep;

  // En passant and castling
  if (PIECE(piece) == PAWN && capture == EMPTY &&
      (orig_pos & 7) != (dest_pos & 7)) {
    int captured_rank = orig_pos >> 3;
    board_set(board, (dest_pos & 7) + captured_rank * 8, EMPTY);
  } else if (PIECE(piece) == KING &&
             abs((orig_pos & 7) - (dest_pos & 7)) == 2) {
    int rook_orig, rook_dest;
    if ((dest_pos & 7) == 5) { // Queen-side castling
      rook_orig = 7 + (orig_pos >> 3) * 8;
      rook_dest = 4 + (orig_pos >> 3) * 8;
    } else { // King-side castling
      rook_orig = 0 + (orig_pos >> 3) * 8;
      rook_dest = 2 + (orig_pos >> 3) * 8;
    }
    board_set(board, rook_dest, board_get(board, rook_orig));
    board_set(board, rook_orig, EMPTY);
  }
  board_set(board, dest_pos, piece);
  board_set(board, orig_pos, EMPTY);

  // Promotion
  if (PIECE(piece) == PAWN) {
    if (COLOR(piece) == WHITE && (dest_pos >> 3) == 7) {
      board_set(board, dest_pos, move->promote);
    } else if (COLOR(piece) == BLACK && (dest_pos >> 3) == 0) {
      board_set(board, dest_pos, move->promote);
    }
  }

  // Switch side
  board->color ^= BLACK;

  // Check if previous king is still in check
  threat_board_update(board);
  if (threat_check(board)) {
    move_undo(board, move, undo);
    return -1;
  }

  // Update castle flags
  if (piece == WHITE_KING) {
    board->castle |= WHITE_CASTLE;
  } else if (piece == BLACK_KING) {
    board->castle |= BLACK_CASTLE;
  } else if (PIECE(piece) == ROOK) {
    switch (orig_pos) {
    case 0:
      board->castle |= WHITE_CASTLE_KINGSIDE;
      break;
    case 7:
      board->castle |= WHITE_CASTLE_QUEENSIDE;
      break;
    case 56:
      board->castle |= BLACK_CASTLE_KINGSIDE;
      break;
    case 63:
      board->castle |= BLACK_CASTLE_QUEENSIDE;
      break;
    }
  }

  // Update en passant flags
  if (PIECE(piece) == PAWN && abs((orig_pos >> 3) - (dest_pos >> 3)) == 2) {
    int ep_square = (orig_pos + dest_pos) / 2;
    board->ep = 0;
    SET_BIT(board->ep, ep_square);
  } else {
    board->ep = 0;
  }

  // board_add_move(orig_board, move); // FIXME: REMOVE THIS (just for testing
  // depth)
  return 0;
}

// Undo a move
int move_undo(Board *board, Move *move, Undo *undo) {
  int orig_pos = move->from;
  int dest_pos = move->to;
  int piece = move->piece;
  int capture = undo->capture;

  // Switch side
  board->color ^= BLACK;

  // Depromotion
  if (move->promote != EMPTY) {
    piece = (COLOR(piece) == WHITE) ? WHITE_PAWN : BLACK_PAWN;
  }

  // Restore original positions
  board_set(board, orig_pos, piece);
  board_set(board, dest_pos, capture);

  // Handle en passant capture
  if (PIECE(piece) == PAWN && capture == EMPTY &&
      (orig_pos & 7) != (dest_pos & 7)) {
    int captured_rank = orig_pos >> 3;
    int captured_piece = (board->color == WHITE) ? BLACK_PAWN : WHITE_PAWN;
    board_set(board, (dest_pos & 7) + captured_rank * 8, captured_piece);
  }
  // Handle castling
  else if (PIECE(piece) == KING && abs((orig_pos & 7) - (dest_pos & 7)) == 2) {
    int rook_orig, rook_dest;
    if ((dest_pos & 7) == 5) { // Queen-side castling
      rook_orig = 7 + (orig_pos >> 3) * 8;
      rook_dest = 4 + (orig_pos >> 3) * 8;
    } else { // King-side castling
      rook_orig = 0 + (orig_pos >> 3) * 8;
      rook_dest = 2 + (orig_pos >> 3) * 8;
    }
    board_set(board, rook_orig, board_get(board, rook_dest));
    board_set(board, rook_dest, EMPTY);
  }

  // Restore castle flags
  board->castle = undo->castle;

  // Restore en passant flags
  board->ep = undo->ep;

  return 0;
}

// Beginning move enumeration //

void init_move_list(MoveList *list) {
  list->count = 0;
  list->capacity = 20;
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
          (board->color == WHITE
               ? ~board->white | (board->black_threat & ~board->white_kings)
               : ~board->black | (board->white_threat & ~board->black_kings));
#else
  valid = KNIGHT_MASKS[pos] &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos), pos, dest_sq, EMPTY};

    add_move(list, move);
    valid &= valid - 1;
  }
}

void rook_possible_move(Board *board, int pos, MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = bb_rook_attacks(board->all, pos) &
          (board->color == WHITE
               ? ~board->white | (board->black_threat & ~board->white_kings)
               : ~board->black | (board->white_threat & ~board->black_kings));
#else
  valid = bb_rook_attacks(board->all, pos) &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos), pos, dest_sq, EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void bishop_possible_move(Board *board, int pos, MoveList *list) {
  Bb valid;
#ifdef MENACE
  valid = bb_bishop_attacks(board->all, pos) &
          (board->color == WHITE
               ? ~board->white | (board->black_threat & ~board->white_kings)
               : ~board->black | (board->white_threat & ~board->black_kings));
#else
  valid = bb_bishop_attacks(board->all, pos) &
          ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos), pos, dest_sq, EMPTY};
    add_move(list, move);

    valid &= valid - 1;
  }
}

void pawn_possible_move(Board *board, int pos, MoveList *list) {
  int piece = board_get(board, pos);
  int direction = (COLOR(piece) == WHITE) ? 1 : -1;
  int start_rank = (COLOR(piece) == WHITE) ? 1 : 6;
  int promote_rank = (COLOR(piece) == WHITE) ? 7 : 0;

  // One Square
  int forward = pos + (direction * 8);
  if (forward >= 0 && forward < 64 && board_get(board, forward) == EMPTY) {
    if ((forward >> 3) == promote_rank) {
      Move moveq = {piece, pos, forward,
                    COLOR(piece) == WHITE ? WHITE_QUEEN : BLACK_QUEEN};
      Move moven = {piece, pos, forward,
                    COLOR(piece) == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT};
      add_move(list, moveq);
      add_move(list, moven);
    } else {
      Move move = {piece, pos, forward, EMPTY};
      add_move(list, move);
    }

    // Two squares
    if ((pos >> 3) == start_rank) {
      int double_forward = forward + (direction * 8);
      if (board_get(board, double_forward) == EMPTY) {
        Move move = {piece, pos, double_forward, EMPTY};
        add_move(list, move);
      }
    }
  }

  // Captures
  for (int side = -1; side <= 1; side += 2) {
    int capture = forward + side;
    if (capture >= 0 && capture < 64 && abs((pos & 7) - (capture & 7)) == 1) {

      // Normal capture
      int target = board_get(board, capture);
      if (target != EMPTY && COLOR(target) != COLOR(piece)) {
        if ((capture >> 3) == promote_rank) {
          Move moveq = {piece, pos, capture,
                        COLOR(piece) == WHITE ? WHITE_QUEEN : BLACK_QUEEN};
          Move moven = {piece, pos, capture,
                        COLOR(piece) == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT};
          add_move(list, moveq);
          add_move(list, moven);
        } else {
          Move move = {piece, pos, capture, EMPTY};
          add_move(list, move);
        }
      }

      // En passant
      if (IS_BIT_SET(board->ep, capture)) {
        Move move = {piece, pos, capture, EMPTY};
        add_move(list, move);
      }
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
             (board->color == WHITE ? ~board->white | board->black_threat
                                    : ~board->black | board->white_threat);
#else
  Bb valid =
      KING_MASKS[pos] & ~(board->color == WHITE ? board->white : board->black);
#endif
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos), pos, dest_sq, EMPTY};
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
#ifdef MENACE
    if (IS_BIT_SET(board->color == WHITE
                       ? board->white | (board->white_threat & board->black)
                       : board->black | (board->black_threat & board->white),
                   i)) {
      any_possible_move(board, i, piece, &ret);
    }
#else
    if (IS_BIT_SET(board->color == WHITE ? board->white : board->black, i)) {
      any_possible_move(board, i, piece, &ret);
    }
#endif
  }
  return ret;
}
