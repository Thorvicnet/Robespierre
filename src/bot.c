#include "bot.h"
#include "bb.h"
#include <math.h>

typedef struct {
  Move mo;
  int value;
} Vmove; // Valued move, useful for the search

void change_score(int *score, Bb bb, int value) {
  // Modifies the score according to the pieces in bb
  int count = __builtin_popcountll(bb);
  *score += count * value;
}

int evaluate(Board *board) {
  // Returns an evaluation of the position, without depth

  // Values attributed to different pieces (could be modified according to the
  // state of the game)
  int pawn_value = 10;
  int knight_value = 30;
  int bishop_value = 50;
  int rook_value = 60;
  int queen_value = 110;

  int king_value = 5000;

  int threat_value = 2;

  if (board->color == WHITE &&
      ((board->white_threat & board->black_kings) != 0))
    return king_value;
  if (board->color == BLACK &&
      ((board->black_threat & board->white_kings) != 0))
    return -king_value;

  int score = 0;

  change_score(&score, board->white_pawns, pawn_value);
  change_score(&score, board->white_knights, knight_value);
  change_score(&score, board->white_bishops, bishop_value);
  change_score(&score, board->white_rooks, rook_value);
  change_score(&score, board->white_queens, queen_value);

  change_score(&score, board->black_pawns, -pawn_value);
  change_score(&score, board->black_knights, -knight_value);
  change_score(&score, board->black_bishops, -bishop_value);
  change_score(&score, board->black_rooks, -rook_value);
  change_score(&score, board->black_queens, -queen_value);

  change_score(&score, board->white_threat, threat_value);
  change_score(&score, board->black_threat, -threat_value);

  if (__builtin_popcountll(KING_MASKS[__builtin_ctzll(board->white_kings)] &
                           board->white_pawns) >= 3) {
    score += 7;
  }
  if (__builtin_popcountll(KING_MASKS[__builtin_ctzll(board->black_kings)] &
                           board->black_pawns) >= 3) {
    score -= 7;
  }

  return score;
}

Vmove choose_with_depth(Board *board, int depth, int alpha, int beta) {
  // Chooses the best move, according to a minimax search with alpha-beta
  // pruning Currently checks if the move is possible even though we know it is
  // - kinda beta is greater than alpha (or else the branch is pruned)

  Move list_moves[MAX_MOVES];
  int moves_count = move_possible(board, list_moves);

  int best_index = 0;
  int best_eval = board->color == WHITE ? -10000 : 10000;

  for (int i = 0; i < moves_count; i++) {
    Board *new_board = board_copy(board);
    Undo undo;
    int res = move_make(new_board, &(list_moves[i]),
                        &undo); // FIXME: NEED TO UNDO THE MOVE NOT CREATE A NEW
                                // BOARD AND FREE IT
    if (res) { // Move not allowed (could lead to discovered check...)
      board_free(new_board);
      continue;
    }

    int eval;
    if (depth == 1)
      eval = evaluate(new_board);
    else
      eval = choose_with_depth(new_board, depth - 1, alpha, beta).value;

    board_free(new_board);

    if ((board->color == WHITE && eval > best_eval) ||
        (board->color == BLACK && eval < best_eval)) {
      best_index = i;
      best_eval = eval;
      if (abs(best_eval) > 5000) {
        best_eval += board->color == WHITE ? depth : -depth;
        Move best_move = list_moves[best_index];
        return (Vmove){best_move, best_eval};
      }
    }

    if (board->color == WHITE)
      alpha = alpha > eval ? alpha : eval;
    else
      beta = beta < eval ? beta : eval;
    if (beta <= alpha)
      break;
  }

  Move best_move = list_moves[best_index];
  return (Vmove){best_move, best_eval};
}

Move choose(Board *board) {
  // Chooses the best move according to the evaluation
  // Currently lacks : iterative deepening

  int sliding_movement = 12;
  int movement_potential =
      __builtin_popcountll(board->black_queens | board->white_queens) *
          sliding_movement * 2 +
      __builtin_popcountll(board->black_rooks | board->black_bishops |
                           board->white_rooks | board->white_bishops) *
          sliding_movement +
      __builtin_popcountll(board->black_pawns | board->white_pawns) +
      __builtin_popcountll(board->white_knights | board->black_knights) * 5 + 8;

  int depth = floor(45.4 / log2((double)movement_potential));

  wprintf(L"- depth: %d\n", depth);
  Vmove t = choose_with_depth(board, depth, -10000, 10000);

  wprintf(L"- eval: %d\n", t.value);

  return t.mo; // currently arbitrary depth of 5
}
