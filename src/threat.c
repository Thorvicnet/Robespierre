#include "threat.h"

void threat_board_update(Board *board) {
  // White threat
  board->white_threat = 0ULL;
  Bb bb = board->white_pawns;
  while (bb) {
    int sq = __builtin_ctzll(bb); // Counts the number of trailing zeros
    board->white_threat |= PAWN_ATTACK_MASKS_WHITE[sq];
    bb &= (bb - 1); // "POP" the LSB
  }
  bb = board->white_rooks;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->white_threat |= bb_rook_attacks(board->all, sq);
    bb &= (bb - 1);
  }
  bb = board->white_bishops;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->white_threat |= bb_bishop_attacks(board->all, sq);
    bb &= (bb - 1);
  }
  bb = board->white_knights;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->white_threat |= KNIGHT_MASKS[sq];
    bb &= (bb - 1);
  }
  bb = board->white_queens;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->white_threat |= bb_rook_attacks(board->all, sq);
    board->white_threat |= bb_bishop_attacks(board->all, sq);
    bb &= (bb - 1);
  }
  bb = board->white_kings; // I assume there is only one king (pretty safe)
  if (!bb) {
    wprintf(L"No king, strange...\n");
    board_info(board);
    board_bb_info(board);
  }
  board->white_threat |= KING_MASKS[__builtin_ctzll(bb)];

  // Black threat
  board->black_threat = 0ULL;
  bb = board->black_pawns;
  while (bb) {
    int sq = __builtin_ctzll(bb); // Counts the number of trailing zeros
    board->black_threat |= PAWN_ATTACK_MASKS_BLACK[sq];
    bb &= (bb - 1); // "POP" the LSB
  }
  bb = board->black_rooks;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->black_threat |= bb_rook_attacks(board->all, sq);
    bb &= (bb - 1);
  }
  bb = board->black_bishops;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->black_threat |= bb_bishop_attacks(board->all, sq);
    bb &= (bb - 1);
  }
  bb = board->black_knights;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->black_threat |= KNIGHT_MASKS[sq];
    bb &= (bb - 1);
  }
  bb = board->black_queens;
  while (bb) {
    int sq = __builtin_ctzll(bb);
    board->black_threat |= bb_rook_attacks(board->all, sq);
    board->black_threat |= bb_bishop_attacks(board->all, sq);
    bb &= (bb - 1);
  }
  bb = board->black_kings; // I assume there is only one king (pretty safe)
  if (!bb) {
    wprintf(L"No king, strange...\n");
    board_info(board);
  }
  board->black_threat |= KING_MASKS[__builtin_ctzll(bb)];
}

bool threat_transitive_check(Board *board, int color) {
  // Returns true if the king of the chosen color is threatened by a piece that
  // is itself threatened
  if (color == WHITE) {
    // White threat
    Bb bb = board->white_pawns & board->black_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb); // Counts the number of trailing zeros
      if (PAWN_ATTACK_MASKS_WHITE[sq] & board->white_kings)
        return true;
      bb &= (bb - 1); // "POP" the LSB
    }
    bb = board->white_rooks & board->black_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (bb_rook_attacks(board->all, sq) & board->white_kings)
        return true;
      bb &= (bb - 1);
    }
    bb = board->white_bishops & board->black_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (bb_bishop_attacks(board->all, sq) & board->white_kings)
        return true;
      bb &= (bb - 1);
    }
    bb = board->white_knights & board->black_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (!(KNIGHT_MASKS[sq] & board->white_kings))
        return true;
      bb &= (bb - 1);
    }
    bb = board->white_queens & board->black_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (bb_rook_attacks(board->all, sq) & board->white_kings)
        return true;
      if (bb_bishop_attacks(board->all, sq) & board->white_kings)
        return true;
      bb &= (bb - 1);
    }
  } else {
    // Black threat
    Bb bb = board->black_pawns & board->white_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb); // Counts the number of trailing zeros
      if (PAWN_ATTACK_MASKS_BLACK[sq] & board->black_kings)
        return true;
      bb &= (bb - 1); // "POP" the LSB
    }
    bb = board->black_rooks & board->white_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (bb_rook_attacks(board->all, sq) & board->black_kings)
        return true;
      bb &= (bb - 1);
    }
    bb = board->black_bishops & board->white_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (bb_bishop_attacks(board->all, sq) & board->black_kings)
        return true;
      bb &= (bb - 1);
    }
    bb = board->black_knights & board->white_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (KNIGHT_MASKS[sq] & board->black_kings)
        return true;
      bb &= (bb - 1);
    }
    bb = board->black_queens & board->white_threat;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      if (bb_rook_attacks(board->all, sq) & board->black_kings)
        return true;
      if (bb_bishop_attacks(board->all, sq) & board->black_kings)
        return true;
      bb &= (bb - 1);
    }
  }
  return false;
}

bool threat_check(Board *board) {
  // Returns true if the king of the previous player is threatened (which is
  // illegal)
#ifdef MENACE
  return (board->color == WHITE ? (board->black_kings & board->white_threat) |
                                      threat_transitive_check(board, BLACK)
                                : (board->white_kings & board->black_threat) |
                                      threat_transitive_check(board, WHITE));
#else
  return (board->color == WHITE ? board->black_kings & board->white_threat
                                : board->white_kings & board->black_threat);
#endif
}
