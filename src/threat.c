#include "threat.h"

bool check_threatened(Board *board, int cell[2], int color, int depth) {
  // Returns true if cell is threatened by pieces of the opposite color at the
  // chosen depth (1 for a direct threat, 2 for an indirect one) En passant
  // undetected yet
  if (depth == 0)
    return false;

  int dirs[16][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, -1}, {-1, 0}, {-1, 1},
                     {-1, 2},  {0, -1}, {0, 1},   {1, -2},  {1, -1}, {1, 0},
                     {1, 1},   {1, 2},  {2, -1},  {2, 1}};

  for (int i = 0; i < 16; i++) {
    int current_cell[2] = {cell[0] + dirs[i][0], cell[1] + dirs[i][1]};
    int dist = abs(dirs[i][0]) + abs(dirs[i][1]);
    int addon_knight =
        dist == 3 ? 9 : 0; // To prevent knights from doing several jumps

    while (current_cell[0] >= 0 && current_cell[1] >= 0 &&
           current_cell[0] < 8 && current_cell[1] < 8) {
      int piece = board_get(board, current_cell[0] + 8 * current_cell[1]);

      if (piece == EMPTY) {
        current_cell[0] += dirs[i][0] + addon_knight;
        current_cell[1] += dirs[i][1];
      }

      else {

        if ((piece & 0xF0) == board->color) {
          if ((dist <= 2 && (piece & 0x0F) == QUEEN) ||
              (dist == 1 && (piece & 0x0F) == BISHOP) ||
              (dist == 2 && (piece & 0x0F) == ROOK) ||
              (dist == 3 && (piece & 0x0F) == KNIGHT)) {
            if (check_threatened(board, current_cell, color, depth - 1))
              return true;
          }
          current_cell[0] = -1;
        }

        else {
          if ((dist <= 2 && (piece & 0x0F) == QUEEN) ||
              (dist == 1 && (piece & 0x0F) == BISHOP) ||
              (dist == 2 && (piece & 0x0F) == ROOK) ||
              (dist == 3 && (piece & 0x0F) == KNIGHT))
            return true;

          current_cell[0] = -1;
        }
      }
    }
  }

  // Insert pawn code here

  if (cell[1] < 6) {

    if (cell[0] > 0) {
      int piece = board_get(board, cell[0] - 1 + 8 * (cell[1] + 1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == BLACK) {
        if (color == WHITE)
          return true;
        if (check_threatened(board, (int[]){cell[0] - 1, cell[1] + 1}, color,
                             depth - 1))
          return true;
      }
    }
    if (cell[0] < 7) {
      int piece = board_get(board, cell[0] + 1 + 8 * (cell[1] + 1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == BLACK) {
        if (color == WHITE)
          return true;
        if (check_threatened(board, (int[]){cell[0] + 1, cell[1] + 1}, color,
                             depth - 1))
          return true;
      }
    }
  }

  if (cell[1] > 1) {

    if (cell[0] > 0) {
      int piece = board_get(board, cell[0] - 1 + 8 * (cell[1] - 1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == WHITE) {
        if (color == BLACK)
          return true;
        if (check_threatened(board, (int[]){cell[0] - 1, cell[1] - 1}, color,
                             depth - 1))
          return true;
      }
    }
    if (cell[0] < 7) {
      int piece = board_get(board, cell[0] + 1 + 8 * (cell[1] - 1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == WHITE) {
        if (color == BLACK)
          return true;
        if (check_threatened(board, (int[]){cell[0] + 1, cell[1] - 1}, color,
                             depth - 1))
          return true;
      }
    }
  }

  return false;
}

Bb threat_board_squares(Board *board, int color) {
  Bb threatened = 0ULL;
  if (color == WHITE) {
    // White threat
    threatened = 0ULL;
    Bb bb = board->white_pawns;
    while (bb) {
      int sq = __builtin_ctzll(bb); // Counts the number of trailing zeros
      threatened |= PAWN_ATTACK_MASKS_WHITE[sq];
      bb &= (bb - 1); // "POP" the LSB
    }
    bb = board->white_rooks;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= bb_rook_attacks(board->all, sq);
      bb &= (bb - 1);
    }
    bb = board->white_bishops;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= bb_bishop_attacks(board->all, sq);
      bb &= (bb - 1);
    }
    bb = board->white_knights;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= KNIGHT_MASKS[sq];
      bb &= (bb - 1);
    }
    bb = board->white_queens;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= bb_rook_attacks(board->all, sq);
      threatened |= bb_bishop_attacks(board->all, sq);
      bb &= (bb - 1);
    }
    bb = board->white_kings; // I assume there is only one king (pretty safe)
    threatened |= KING_MASKS[__builtin_ctzll(bb)];

  } else {
    // Black threat
    threatened = 0ULL;
    Bb bb = board->black_pawns;
    while (bb) {
      int sq = __builtin_ctzll(bb); // Counts the number of trailing zeros
      threatened |= PAWN_ATTACK_MASKS_BLACK[sq];
      bb &= (bb - 1); // "POP" the LSB
    }
    bb = board->black_rooks;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= bb_rook_attacks(board->all, sq);
      bb &= (bb - 1);
    }
    bb = board->black_bishops;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= bb_bishop_attacks(board->all, sq);
      bb &= (bb - 1);
    }
    bb = board->black_knights;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= KNIGHT_MASKS[sq];
      bb &= (bb - 1);
    }
    bb = board->black_queens;
    while (bb) {
      int sq = __builtin_ctzll(bb);
      threatened |= bb_rook_attacks(board->all, sq);
      threatened |= bb_bishop_attacks(board->all, sq);
      bb &= (bb - 1);
    }
    bb = board->black_kings; // I assume there is only one king (pretty safe)
    threatened |= KING_MASKS[__builtin_ctzll(bb)];
  }
  return threatened;
}

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
