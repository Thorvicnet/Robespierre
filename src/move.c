#include "move.h"

// The first part of this file is for move validation and should only be used
// when playing locally against a player, the second part is for possible move
// enumeration for the bot

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
    if (board->ep & (1ULL << dest) && board_get(board, dest) == EMPTY) {
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

int move_make(Board *board, Move *move, Undo *undo) {
  // Move a piece on the board, returns -1 if the move is invalid (king still in
  // check)
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
    board_set_empty(board, (dest_pos & 7) + captured_rank * 8, piece ^ 0xF0);
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
    int rook = board_get(board, rook_orig);
    board_set(board, rook_dest, rook);
    board_set_empty(board, rook_orig, rook);
  }
  board_set(board, dest_pos, piece);
  board_set_empty(board, orig_pos, piece);

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

  return 0;
}

int move_undo(Board *board, Move *move, Undo *undo) {
  // Undo a move
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
    int rook = board_get(board, rook_dest);
    board_set(board, rook_orig, rook);
    board_set_empty(board, rook_dest, rook);
  }

  // Restore castle flags
  board->castle = undo->castle;

  // Restore en passant flags
  board->ep = undo->ep;

  return 0;
}

// Beginning move enumeration //

int knight_possible_move(Board *board, int pos, Move *list) {
  int count = 0;
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
    list[count++] = move;
    valid &= valid - 1;
  }
  return count;
}

int rook_possible_move(Board *board, int pos, Move *list) {
  int count = 0;
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
    list[count++] = move;
    valid &= valid - 1;
  }
  return count;
}

int bishop_possible_move(Board *board, int pos, Move *list) {
  int count = 0;
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
    list[count++] = move;
    valid &= valid - 1;
  }
  return count;
}

int pawn_possible_move(Board *board, int pos, Move *list) {
  int count = 0;
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
      list[count++] = moveq;
      list[count++] = moven;
    } else {
      Move move = {piece, pos, forward, EMPTY};
      list[count++] = move;
    }

    // Two squares
    if ((pos >> 3) == start_rank) {
      int double_forward = forward + (direction * 8);
      if (board_get(board, double_forward) == EMPTY) {
        Move move = {piece, pos, double_forward, EMPTY};
        list[count++] = move;
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
          list[count++] = moveq;
          list[count++] = moven;
        } else {
          Move move = {piece, pos, capture, EMPTY};
          list[count++] = move;
        }
      }

      // En passant
      if (IS_BIT_SET(board->ep, capture)) {
        Move move = {piece, pos, capture, EMPTY};
        list[count++] = move;
      }
    }
  }
  return count;
}

int queen_possible_move(Board *board, int pos, Move *list) {
  int count = 0;
  count += rook_possible_move(board, pos, list);
  count += bishop_possible_move(board, pos, &list[count]);
  return count;
}

int king_possible_move(Board *board, int pos, Move *list) {
  int count = 0;
  Bb valid = KING_MASKS[pos] &
             ~(board->color == WHITE ? board->white | board->black_threat
                                     : board->black | board->white_threat);
  while (valid) {
    int dest_sq = __builtin_ctzll(valid);

    Move move = {board_get(board, pos), pos, dest_sq, EMPTY};
    list[count++] = move;
    valid &= valid - 1;
  }

  // Castling
  if ((board->color == WHITE && pos == 3) ||
      (board->color == BLACK && pos == 59)) {

    Bb threats =
        (board->color == WHITE) ? board->black_threat : board->white_threat;
    int rank = pos >> 3;

    // Kingside castle
    if (!(board->castle & (board->color == WHITE ? WHITE_CASTLE_KINGSIDE
                                                 : BLACK_CASTLE_KINGSIDE))) {
      Bb kingside_path = ((3ULL) << (rank * 8 + 1));
      if (!(board->all & kingside_path) && !(threats & kingside_path)) {
        Move move = {board_get(board, pos), pos, pos - 2, EMPTY};
        list[count++] = move;
      }
    }

    // Queenside castle
    if (!(board->castle & (board->color == WHITE ? WHITE_CASTLE_QUEENSIDE
                                                 : BLACK_CASTLE_QUEENSIDE))) {
      Bb queenside_path = ((7ULL) << (rank * 8 + 4));
      if (!(board->all & queenside_path) &&
          !(threats & (queenside_path >> 1))) {
        Move move = {board_get(board, pos), pos, pos + 2, EMPTY};
        list[count++] = move;
      }
    }
  }
  return count;
}

int move_possible(Board *board, Move *moves) {
  // Fill the move array with all possible moves (pseudo legal generation) and
  // returns the move count
  int count = 0;
  Bb pieces;

  // Queens
#ifdef MENACE
  pieces =
      board->color == WHITE
          ? board->white_queens | (board->white_threat & board->black_queens)
          : board->black_queens | (board->black_threat & board->white_queens);
#else
  pieces = board->color == WHITE ? board->white_queens : board->black_queens;
#endif
  while (pieces) {
    int pos = __builtin_ctzll(pieces);
    count += queen_possible_move(board, pos, &moves[count]);
    pieces &= pieces - 1;
  }

  // Rooks
#ifdef MENACE
  pieces =
      board->color == WHITE
          ? board->white_rooks | (board->white_threat & board->black_rooks)
          : board->black_rooks | (board->black_threat & board->white_rooks);
#else
  pieces = board->color == WHITE ? board->white_rooks : board->black_rooks;
#endif
  while (pieces) {
    int pos = __builtin_ctzll(pieces);
    count += rook_possible_move(board, pos, &moves[count]);
    pieces &= pieces - 1;
  }

  // Bishops
#ifdef MENACE
  pieces =
      board->color == WHITE
          ? board->white_bishops | (board->white_threat & board->black_bishops)
          : board->black_bishops | (board->black_threat & board->white_bishops);
#else
  pieces = board->color == WHITE ? board->white_bishops : board->black_bishops;
#endif
  while (pieces) {
    int pos = __builtin_ctzll(pieces);
    count += bishop_possible_move(board, pos, &moves[count]);
    pieces &= pieces - 1;
  }

  // Knights
#ifdef MENACE
  pieces =
      board->color == WHITE
          ? board->white_knights | (board->white_threat & board->black_knights)
          : board->black_knights | (board->black_threat & board->white_knights);
#else
  pieces = board->color == WHITE ? board->white_knights : board->black_knights;
#endif
  while (pieces) {
    int pos = __builtin_ctzll(pieces);
    count += knight_possible_move(board, pos, &moves[count]);
    pieces &= pieces - 1;
  }

  // Pawns
#ifdef MENACE
  pieces =
      board->color == WHITE
          ? board->white_pawns | (board->white_threat & board->black_pawns)
          : board->black_pawns | (board->black_threat & board->white_pawns);
#else
  pieces = board->color == WHITE ? board->white_pawns : board->black_pawns;
#endif
  while (pieces) {
    int pos = __builtin_ctzll(pieces);
    count += pawn_possible_move(board, pos, &moves[count]);
    pieces &= pieces - 1;
  }

  // Kings
#ifdef MENACE
  pieces =
      board->color == WHITE
          ? board->white_kings | (board->white_threat & board->black_kings)
          : board->black_kings | (board->black_threat & board->white_kings);
#else
  pieces = board->color == WHITE ? board->white_kings : board->black_kings;
#endif
  while (pieces) {
    int pos = __builtin_ctzll(pieces);
    count += king_possible_move(board, pos, &moves[count]);
    pieces &= pieces - 1;
  }

  return count;
}
