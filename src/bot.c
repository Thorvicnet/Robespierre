#include "bot.h"

typedef struct {
  Move mo;
  int value;
} Vmove; // Valued move, useful for the search

int check_endgame(MoveTree *tree){
  //Returns 0, 5000, -5000 or 1 if the current static tree is a draw/checkmate/other
  //For now, draw only means stalemate
  //This function should only be called on certified leafs (positions that don't have any children)
  if (!tree -> children_filled || tree -> moves.count > 0) {
    wprintf(L"Not an endgame position\n");
    exit(1);
  }
  if (tree->board.white_threat & tree->board.black_kings) return 5000;
  if (tree->board.black_threat & tree->board.white_kings) return -5000;
  return 0;
}

void change_score(int *score, Bb bb, int value) {
  // Modifies the score according to the pieces in bb
  int count = __builtin_popcountll(bb);
  *score += count * value;
}

int evaluate(Board *board) {
  // Returns a static evaluation of the position (without depth)

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

  return score;
}

Vmove choose_with_depth(Board *board, int depth, int alpha, int beta) {
  // Chooses the best move, according to a minimax search with alpha-beta
  // pruning Currently checks if the move is possible even though we know it is
  // - kinda beta is greater than alpha (or else the branch is pruned)

  MoveList list_moves =
      move_possible(board); // Should be a list of every valid move
  if (list_moves.count <= 0) {
    move_list_free(&list_moves);
    exit(2);
  }; // Checkmate, draw ... to be dealt with later

  int best_index = 0;
  int best_eval = board->color == WHITE ? -10000 : 10000;

  for (int i = 0; i < list_moves.count; i++) {
    Board *new_board = board_copy(board);
    int res = move(new_board, list_moves.moves[i]);
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
      if (abs(eval) >= 4000) best_eval = board->color == WHITE ? eval-1:eval+1;
    }

    if (board->color == WHITE)
      alpha = alpha > eval ? alpha : eval;
    else
      beta = beta < eval ? beta : eval;
    if (beta <= alpha)
      break;
  }
  if (best_eval > 5000) best_eval -= 5000;
  Move best_move = list_moves.moves[best_index];
  move_list_free(&list_moves);
  return (Vmove){best_move, best_eval};
}

int choose_with_trees(MoveTree *tree, int depth, int alpha, int beta, time_t time_at_start, double max_allowed) {
  // Returns the best evaluation of the position, modifies tree while doing so
  // pruning Currently checks if the move is possible even though we know it is - knida
  // beta is greater than alpha (or else the branch is pruned)
  // Stops if too much time has passed

  if (depth == 0){
    return evaluate(&tree -> board);
  }

  if (!tree -> children_filled) create_tree_children(tree);
  if (tree -> moves.count == 0) return check_endgame(tree);

  int best_eval = tree->board.color == WHITE ? -10000 : 10000;

  for (int i = 0; i < tree -> moves.count; i++) {

    int eval = choose_with_trees(tree -> children[i], depth - 1, alpha, beta, time_at_start, max_allowed);

    if ((tree->board.color == WHITE && eval > best_eval) ||
        (tree->board.color == BLACK && eval < best_eval)) {
      best_eval = eval;
      tree_rotation(tree, i);
      //if (abs(eval) >= 4000) best_eval = board->color == WHITE ? eval-1:eval+1;
    }

    if (tree->board.color == WHITE)
      alpha = alpha > eval ? alpha : eval;
    else
      beta = beta < eval ? beta : eval;
    if (beta <= alpha)
      break;

    time_t current_time;
    time(&current_time);
    if (difftime(current_time, time_at_start) > 5) exit(1);
    if (difftime(current_time, time_at_start) > max_allowed) break;
  }

  return best_eval;
}

Move choose(Board *board) {
  // Chooses the best move according to the evaluation
  // Currently lacks : iterative deepening

  Vmove t = choose_with_depth(board, 7, -10000, 10000);

  wprintf(L"- eval: %d\n", t.value);

  return t.mo; // currently arbitrary depth of 5
}

Move choose2(MoveTree *tree){
  //Chooses the best move according to the evaluation
  //Used to replace choose with garden management
  int eval;

  time_t time_at_start;
  time(&time_at_start);
  time_t current_time = time_at_start;
  double max_allowed = 2;

  int i = 1;
  while (difftime(current_time, time_at_start) < max_allowed){
    eval = choose_with_trees(tree, i, -10000, 10000, time_at_start, max_allowed);
    i++;
    time(&current_time);
  }

  wprintf(L"- depth : %d\n- eval : %d\n", i, eval);
  return tree -> moves.moves[0];
}
