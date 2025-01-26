#include "bot.h"

const int white_bishop_eval[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,
    0,   0,   -10, -10, 0,   5,   10,  10,  5,   0,   -10, -10, 5,
    5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  0,
    -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 5,   0,   0,
    0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20};

const int black_bishop_eval[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 5,   0,   0,   0,
    0,   5,   -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 0,
    10,  10,  10,  10,  0,   -10, -10, 5,   5,   10,  10,  5,   5,
    -10, -10, 0,   5,   10,  10,  5,   0,   -10, -10, 0,   0,   0,
    0,   0,   0,   -10, -20, -10, -10, -10, -10, -10, -10, -20};

const int white_knight_eval[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,
    0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
    15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
    -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   5,
    5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50};

const int black_knight_eval[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   5,   5,
    0,   -20, -40, -30, 5,   10,  15,  15,  10,  5,   -30, -30, 0,
    15,  20,  20,  15,  0,   -30, -30, 5,   15,  20,  20,  15,  5,
    -30, -30, 0,   10,  15,  15,  10,  0,   -30, -40, -20, 0,   0,
    0,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50};

const int whire_rook_eval[64] = {
    0,   0, 0, 0, 0, 0, 0, 0,   10,  10, 10, 10, 10, 10, 10, 10,
    -10, 0, 0, 0, 0, 0, 0, -10, -10, 0,  0,  0,  0,  0,  0,  -10,
    -10, 0, 0, 0, 0, 0, 0, -10, -10, 0,  0,  0,  0,  0,  0,  -10,
    -10, 0, 0, 0, 0, 0, 0, -10, 0,   0,  0,  10, 10, 0,  0,  0};

const int black_rook_eval[64] = {
    0,   0,  0,  10, 10, 0,  0,  0,   -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0,  0,  0,  0,  0,  0,  -10, -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0,  0,  0,  0,  0,  0,  -10, -10, 0, 0, 0, 0, 0, 0, -10,
    10,  10, 10, 10, 10, 10, 10, 10,  0,   0, 0, 0, 0, 0, 0, 0};

const int white_queen_eval[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20, -10, 0,   0,   0,  0,  0,   0,   -10,
    -10, 0,   5,   5,  5,  5,   0,   -10, -5,  0,   5,   5,  5,  5,   0,   -5,
    0,   0,   5,   5,  5,  5,   0,   -5,  -10, 5,   5,   5,  5,  5,   0,   -10,
    -10, 0,   5,   0,  0,  0,   0,   -10, -20, -10, -10, -5, -5, -10, -10, -20};

const int black_queen_eval[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20, -10, 0,   5,   0,  0,  0,   0,   -10,
    -10, 5,   5,   5,  5,  5,   0,   -10, 0,   0,   5,   5,  5,  5,   0,   -5,
    -5,  0,   5,   5,  5,  5,   0,   -5,  -10, 0,   5,   5,  5,  5,   0,   -10,
    -10, 0,   0,   0,  0,  0,   0,   -10, -20, -10, -10, -5, -5, -10, -10, -20};

typedef struct {
  Move moves[MAX_MOVES];
  int move_count;
} MoveBranch;

int check_endgame(Board *board) {
  // Returns 0, 5000, -5000 or 1 if the current static tree is a
  // draw/checkmate/other For now, draw only means stalemate This function
  // should only be called on certified leafs (positions that don't have any
  // children)
  Move moves[MAX_MOVES];
  int move_count = move_possible(board, moves);

  if (move_count > 0) {
    wprintf(L"Not an endgame position\n");
    exit(1);
  }

  if ((board->color == WHITE && (board->white_threat & board->black_kings)) ||
      (board->color == BLACK && (board->black_threat & board->white_kings))) {
    return (board->color == WHITE) ? -5000 : 5000;
  }

  return 1;
}

void change_score(int *score, Bb bb, int value) {
  // Modifies the score according to the pieces in bb
  int count = __builtin_popcountll(bb);
  *score += count * value;
}

void attribute_score_from_pose(int *score, Bb bb, const int pos_value[64]) {
  // fastest way i found
  if (bb == 0)
    return;
  int first_pieces = __builtin_ctzll(bb);
  int second_pieces = 63 - __builtin_clzll(bb);
  if (first_pieces <= 63 && first_pieces >= 0)
    *score += pos_value[first_pieces] / 10;
  if (second_pieces <= 63 && second_pieces >= 0 &&
      first_pieces != second_pieces)
    *score += pos_value[first_pieces] / 10;
}

int evaluate(Board *board) {
  // Returns a static evaluation of the position, without depth
  // ATM it is very basic

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

  attribute_score_from_pose(&score, board->black_bishops, black_bishop_eval);
  attribute_score_from_pose(&score, board->white_bishops, white_bishop_eval);
  attribute_score_from_pose(&score, board->black_knights, black_knight_eval);
  attribute_score_from_pose(&score, board->white_knights, white_knight_eval);
  attribute_score_from_pose(&score, board->black_rooks, black_rook_eval);
  attribute_score_from_pose(&score, board->white_rooks, whire_rook_eval);
  attribute_score_from_pose(&score, board->black_queens, black_queen_eval);
  attribute_score_from_pose(&score, board->white_queens, white_queen_eval);

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

int alpha_beta(Board *board, int depth, int alpha, int beta, Move *pv,
               time_t start_time, double max_allowed) {
  if (depth == 0) {
    return evaluate(board);
  }

  MoveBranch branch;
  branch.move_count = move_possible(board, branch.moves);
  if (branch.move_count == 0) {
    return check_endgame(board);
  }

  int best_eval = board->color == WHITE ? -10000 : 10000;
  Move best_move;

  for (int i = 0; i < branch.move_count; i++) {
    Undo undo;
    if (move_make(board, &branch.moves[i], &undo)) {
      continue; // Skip invalid moves
    }

    Move line[MAX_DEPTH];
    int eval = alpha_beta(board, depth - 1, alpha, beta, line, start_time,
                          max_allowed);
    move_undo(board, &branch.moves[i], &undo);

    if ((board->color == WHITE && eval > best_eval) ||
        (board->color == BLACK && eval < best_eval)) {
      best_eval = eval;
      best_move = branch.moves[i];
      pv[0] = branch.moves[i];
      memcpy(&pv[1], line, (depth - 1) * sizeof(Move));
    }

    if (board->color == WHITE) {
      alpha = alpha > eval ? alpha : eval;
    } else {
      beta = beta < eval ? beta : eval;
    }

    if (beta <= alpha) {
      break;
    }

    // Check if time has run out
    time_t current_time;
    time(&current_time);
    if (difftime(current_time, start_time) > max_allowed) {
      break;
    }
  }

  return best_eval;
}

int iterative_deepening(Board *board, Move *best_move, double max_allowed) {
  Move pv[MAX_DEPTH];
  int eval;
  time_t start_time;
  time(&start_time);

  for (int depth = 1; depth < MAX_DEPTH; depth++) {
    eval = alpha_beta(board, depth, -10000, 10000, pv, start_time, max_allowed);

    *best_move = pv[0];

    // Check if time has run out
    time_t current_time;
    time(&current_time);
    if (difftime(current_time, start_time) > max_allowed) {
      break;
    }
  }

  return eval;
}
