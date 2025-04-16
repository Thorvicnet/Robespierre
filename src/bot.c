#include "bot.h"

const int white_bishop_eval[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,
    0,   0,   -10, -10, 0,   5,   10,  10,  5,   0,   -10, -10, 5,
    5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  10,
    0,   -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 5,    0,
    0,   0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20};

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

const int white_rook_eval[64] = {
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

#define TT_EXACT 0
#define TT_UPPER 1
#define TT_LOWER 2

typedef struct {
  uint64_t key;
  int depth;
  int flag;
  int score;
  Move best_move;
} TTEntry;

#define TT_SIZE (1024 * 1024)
TTEntry *transposition_table = NULL;

uint64_t zobrist_keys[12][64];
uint64_t zobrist_side;
uint64_t zobrist_ep[8];
uint64_t zobrist_castling[16];

void init_zobrist() {
  for (int piece = 0; piece < 12; piece++) {
    for (int square = 0; square < 64; square++) {
      zobrist_keys[piece][square] = ((uint64_t)rand() << 32) | rand();
    }
  }
  
  zobrist_side = ((uint64_t)rand() << 32) | rand();
  
  for (int i = 0; i < 8; i++) {
    zobrist_ep[i] = ((uint64_t)rand() << 32) | rand();
  }
  
  for (int i = 0; i < 16; i++) {
    zobrist_castling[i] = ((uint64_t)rand() << 32) | rand();
  }
}

uint64_t calculate_hash(Board *board) {
  uint64_t hash = 0;
  
  for (int square = 0; square < 64; square++) {
    int piece = board->squares[square];
    if (piece != EMPTY) {
      int piece_index = (PIECE(piece) - 1) + (COLOR(piece) == WHITE ? 0 : 6);
      hash ^= zobrist_keys[piece_index][square];
    }
  }
  
  if (board->color == BLACK) {
    hash ^= zobrist_side;
  }
  
  if (board->ep) {
    int ep_file = __builtin_ctzll(board->ep) & 7;
    hash ^= zobrist_ep[ep_file];
  }
  
  hash ^= zobrist_castling[board->castle & 0xF];
  
  return hash;
}

void init_tt() {
  if (transposition_table == NULL) {
    transposition_table = (TTEntry *)calloc(TT_SIZE, sizeof(TTEntry));
    init_zobrist();
  } else {
    memset(transposition_table, 0, TT_SIZE * sizeof(TTEntry));
  }
}

void free_tt() {
  if (transposition_table != NULL) {
    free(transposition_table);
    transposition_table = NULL;
  }
}

void store_tt_entry(uint64_t key, int depth, int score, int flag, Move best_move) {
  int index = key % TT_SIZE;
  TTEntry *entry = &transposition_table[index];
  
  if (entry->key == 0 || entry->depth <= depth || entry->key == key) {
    entry->key = key;
    entry->depth = depth;
    entry->score = score;
    entry->flag = flag;
    entry->best_move = best_move;
  }
}

TTEntry *probe_tt(uint64_t key) {
  int index = key % TT_SIZE;
  TTEntry *entry = &transposition_table[index];
  
  if (entry->key == key) {
    return entry;
  }
  
  return NULL;
}

typedef struct {
  Move moves[MAX_MOVES];
  int move_count;
} MoveBranch;

// Piece values for move ordering and SEE
const int piece_values[7] = {0, 100, 300, 500, 600, 1100, 10000};

int get_piece_value(int piece) {
  if (piece == EMPTY) return 0;
  
  switch (PIECE(piece)) {
  case PAWN: return piece_values[1];
  case KNIGHT: return piece_values[2];
  case BISHOP: return piece_values[3];
  case ROOK: return piece_values[4];
  case QUEEN: return piece_values[5];
  case KING: return piece_values[6];
  default: return 0;
  }
}

int score_capture(Board *board, Move *move) {
  int victim = board_get(board, move->to);
  
  if (victim == EMPTY) {
    // For en passant captures
    if (PIECE(move->piece) == PAWN && (move->from & 7) != (move->to & 7) && 
        IS_BIT_SET(board->ep, move->to)) {
      return piece_values[1] * 10;
    }
    return 0;
  }
  
  // MVV-LVA (Most Valuable Victim - Least Valuable Aggressor)
  return get_piece_value(victim) * 10 - get_piece_value(move->piece);
}

int see(Board *board, int square, int side, int target_value) {
  int value = 0;
  int piece_value = target_value;
  Bb attackers;
  
  // Find all pieces that attack this square
  if (side == WHITE) {
    attackers = 
      (KNIGHT_MASKS[square] & board->black_knights) |
      (bb_bishop_attacks(board->all, square) & (board->black_bishops | board->black_queens)) |
      (bb_rook_attacks(board->all, square) & (board->black_rooks | board->black_queens)) |
      (KING_MASKS[square] & board->black_kings);
      
    // Add pawn attackers
    if (square > 7) {
      if ((square & 7) > 0 && IS_BIT_SET(board->black_pawns, square - 9)) {
        attackers |= (1ULL << (square - 9));
      }
      if ((square & 7) < 7 && IS_BIT_SET(board->black_pawns, square - 7)) {
        attackers |= (1ULL << (square - 7));
      }
    }
  } else {
    attackers = 
      (KNIGHT_MASKS[square] & board->white_knights) |
      (bb_bishop_attacks(board->all, square) & (board->white_bishops | board->white_queens)) |
      (bb_rook_attacks(board->all, square) & (board->white_rooks | board->white_queens)) |
      (KING_MASKS[square] & board->white_kings);
      
    // Add pawn attackers
    if (square < 56) {
      if ((square & 7) > 0 && IS_BIT_SET(board->white_pawns, square + 7)) {
        attackers |= (1ULL << (square + 7));
      }
      if ((square & 7) < 7 && IS_BIT_SET(board->white_pawns, square + 9)) {
        attackers |= (1ULL << (square + 9));
      }
    }
  }
  
  if (attackers == 0) {
    return value;
  }
  
  // Find the least valuable attacker
  int lva = 6;
  int lva_square = -1;
  
  Bb piece_bb;
  
  // Check pawns first
  piece_bb = side == WHITE ? board->black_pawns : board->white_pawns;
  if (piece_bb & attackers) {
    lva = 1;
    lva_square = __builtin_ctzll(piece_bb & attackers);
  } else {
    // Check knights
    piece_bb = side == WHITE ? board->black_knights : board->white_knights;
    if (piece_bb & attackers) {
      lva = 2;
      lva_square = __builtin_ctzll(piece_bb & attackers);
    } else {
      // Check bishops
      piece_bb = side == WHITE ? board->black_bishops : board->white_bishops;
      if (piece_bb & attackers) {
        lva = 3;
        lva_square = __builtin_ctzll(piece_bb & attackers);
      } else {
        // Check rooks
        piece_bb = side == WHITE ? board->black_rooks : board->white_rooks;
        if (piece_bb & attackers) {
          lva = 4;
          lva_square = __builtin_ctzll(piece_bb & attackers);
        } else {
          // Check queens
          piece_bb = side == WHITE ? board->black_queens : board->white_queens;
          if (piece_bb & attackers) {
            lva = 5;
            lva_square = __builtin_ctzll(piece_bb & attackers);
          } else {
            // Must be king
            lva = 6;
            lva_square = __builtin_ctzll(side == WHITE ? board->black_kings : board->white_kings);
          }
        }
      }
    }
  }
  
  value = piece_value - piece_values[lva];
  
  if (value < 0) {
    return value;
  }
  
  // Recursive evaluation
  int see_value = -see(board, square, side ^ BLACK, piece_values[lva]);
  
  return value > see_value ? value : see_value;
}

int is_good_capture(Board *board, Move *move) {
  int victim = board_get(board, move->to);
  
  if (victim == EMPTY) {
    // Check for en passant
    if (PIECE(move->piece) == PAWN && (move->from & 7) != (move->to & 7) && 
        IS_BIT_SET(board->ep, move->to)) {
      return 1;
    }
    return 0;
  }
  
  // Quick check for obviously good captures
  int aggressor_value = get_piece_value(move->piece);
  int victim_value = get_piece_value(victim);
  
  if (victim_value > aggressor_value) {
    return 1;
  }
  
  // For equal or lower value captures, use SEE
  int see_score = see(board, move->to, board->color ^ BLACK, victim_value);
  return see_score >= 0;
}

void sort_captures(Move *moves, int *scores, int count) {
  for (int i = 0; i < count - 1; i++) {
    for (int j = i + 1; j < count; j++) {
      if (scores[j] > scores[i]) {
        // Swap moves
        Move temp_move = moves[i];
        moves[i] = moves[j];
        moves[j] = temp_move;
        
        // Swap scores
        int temp_score = scores[i];
        scores[i] = scores[j];
        scores[j] = temp_score;
      }
    }
  }
}

void change_score(int *score, Bb bb, int value) {
  // Modifies the score according to the pieces in bb
  int count = __builtin_popcountll(bb);
  *score += count * value;
}

void change_score_from_pos(int *score, Bb bb, const int pos_value[64]) {
  while (bb) {
    int square = __builtin_ctzll(bb);
    *score += pos_value[square];
    bb &= bb - 1;
  }
}

int evaluate(Board *board) {
  // Returns a static evaluation of the position, without depth
  // ATM it is very basic

  // Values attributed to different pieces (could be modified according to the
  // state of the game)
  int pawn_value = 100;
  int knight_value = 300;
  int bishop_value = 500;
  int rook_value = 600;
  int queen_value = 1100;

  int threat_value = 10;

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

  change_score_from_pos(&score, board->black_bishops, black_bishop_eval);
  change_score_from_pos(&score, board->white_bishops, white_bishop_eval);
  change_score_from_pos(&score, board->black_knights, black_knight_eval);
  change_score_from_pos(&score, board->white_knights, white_knight_eval);
  change_score_from_pos(&score, board->black_rooks, black_rook_eval);
  change_score_from_pos(&score, board->white_rooks, white_rook_eval);
  change_score_from_pos(&score, board->black_queens, black_queen_eval);
  change_score_from_pos(&score, board->white_queens, white_queen_eval);

  if (__builtin_popcountll(KING_MASKS[__builtin_ctzll(board->white_kings)] &
                           board->white_pawns) >= 3) {
    score += 60;
  }
  if (__builtin_popcountll(KING_MASKS[__builtin_ctzll(board->black_kings)] &
                           board->black_pawns) >= 3) {
    score -= -60;
  }

  score += (rand() % 11) - 5;

  return score;
}

int quiescence_search(Board *board, int alpha, int beta, time_t start_time, double max_allowed) {
  int stand_pat = evaluate(board);
  
  if (board->color == WHITE) {
    if (stand_pat >= beta)
      return beta;
    if (stand_pat > alpha)
      alpha = stand_pat;
  } else {
    if (stand_pat <= alpha)
      return alpha;
    if (stand_pat < beta)
      beta = stand_pat;
  }
  
  // Generate moves
  Move moves[MAX_MOVES];
  int move_count = move_possible(board, moves);
  
  // Time check
  time_t current_time;
  time(&current_time);
  if (difftime(current_time, start_time) > max_allowed) {
    return stand_pat;
  }
  
  // Score and filter capture moves
  Move captures[MAX_MOVES];
  int scores[MAX_MOVES];
  int capture_count = 0;
  
  for (int i = 0; i < move_count; i++) {
    // Skip non-captures
    if (board_get(board, moves[i].to) == EMPTY && 
        !(PIECE(moves[i].piece) == PAWN && (moves[i].from & 7) != (moves[i].to & 7) && 
          IS_BIT_SET(board->ep, moves[i].to))) {
      continue;
    }
    
    // Skip bad captures using SEE pruning
    if (!is_good_capture(board, &moves[i])) {
      continue;
    }
    
    captures[capture_count] = moves[i];
    scores[capture_count] = score_capture(board, &moves[i]);
    capture_count++;
  }
  
  // Sort captures by score
  sort_captures(captures, scores, capture_count);
  
  // Search the captures
  for (int i = 0; i < capture_count; i++) {
    Undo undo;
    if (move_make(board, &captures[i], &undo)) {
      continue;
    }
    
    int score = -quiescence_search(board, -beta, -alpha, start_time, max_allowed);
    move_undo(board, &captures[i], &undo);
    
    if (score >= beta)
      return beta;
    if (score > alpha)
      alpha = score;
  }
  
  return alpha;
}

int alpha_beta(Board *board, int depth, int alpha, int beta, Move *pv,
               time_t start_time, double max_allowed) {
  if (depth == 0) {
    return quiescence_search(board, alpha, beta, start_time, max_allowed);
  }
  
  // Check for time
  time_t current_time;
  time(&current_time);
  if (difftime(current_time, start_time) > max_allowed) {
    return board->color == WHITE ? alpha : beta;
  }
  
  // Calculate position hash
  uint64_t pos_key = calculate_hash(board);
  
  // Probe transposition table
  TTEntry *tt_entry = probe_tt(pos_key);
  if (tt_entry != NULL && tt_entry->depth >= depth) {
    // Use TT entry if its depth is sufficient
    if (tt_entry->flag == TT_EXACT) {
      if (pv) {
        pv[0] = tt_entry->best_move;
        memset(&pv[1], 0, (depth - 1) * sizeof(Move));
      }
      return tt_entry->score;
    } else if (tt_entry->flag == TT_LOWER && tt_entry->score >= beta) {
      return tt_entry->score;
    } else if (tt_entry->flag == TT_UPPER && tt_entry->score <= alpha) {
      return tt_entry->score;
    }
  }

  MoveBranch branch;
  branch.move_count = move_possible(board, branch.moves);
  
  // Move ordering: try TT move first
  if (tt_entry != NULL) {
    for (int i = 0; i < branch.move_count; i++) {
      if (branch.moves[i].from == tt_entry->best_move.from && 
          branch.moves[i].to == tt_entry->best_move.to &&
          branch.moves[i].promote == tt_entry->best_move.promote) {
        // Swap to put the best move first
        Move temp = branch.moves[0];
        branch.moves[0] = branch.moves[i];
        branch.moves[i] = temp;
        break;
      }
    }
  }

  int best_eval = board->color == WHITE ? -10000 : 10000;
  Move best_move = {0};
  bool foundmove = false;
  int orig_alpha = alpha;

  for (int i = 0; i < branch.move_count; i++) {
    Undo undo;
    if (move_make(board, &branch.moves[i], &undo)) {
      continue; // Skip invalid moves
    }

    foundmove = true;

    Move line[MAX_DEPTH];
    int eval = -alpha_beta(board, depth - 1, -beta, -alpha, line, start_time,
                          max_allowed);
    move_undo(board, &branch.moves[i], &undo);

    if ((board->color == WHITE && eval > best_eval) ||
        (board->color == BLACK && eval < best_eval)) {
      best_eval = eval;
      best_move = branch.moves[i];
      
      if (pv) {
        pv[0] = branch.moves[i];
        memcpy(&pv[1], line, (depth - 1) * sizeof(Move));
      }
    }

    if (board->color == WHITE) {
      alpha = alpha > eval ? alpha : eval;
    } else {
      beta = beta < eval ? beta : eval;
    }

    if (beta <= alpha) {
      break;
    }
  }

  if (!foundmove) {
    if ((board->color == WHITE && (board->white_kings & board->black_threat)) ||
        (board->color == BLACK && (board->black_kings & board->white_threat))) {
      // Checkmate
      best_eval = (board->color == WHITE) ? -5000 - depth : 5000 + depth;
    } else {
      // Stalemate
      best_eval = 0;
    }
    if (pv) {
      memset(pv, 0, depth * sizeof(Move));
    }
  } else {
    // Store position in transposition table
    int flag;
    if (best_eval <= orig_alpha) {
      flag = TT_UPPER;
    } else if (best_eval >= beta) {
      flag = TT_LOWER;
    } else {
      flag = TT_EXACT;
    }
    
    store_tt_entry(pos_key, depth, best_eval, flag, best_move);
  }

  return best_eval;
}

int iterative_deepening(Board *board, Move *best_move, double max_allowed) {
  Move pv[MAX_DEPTH];
  int eval;
  time_t start_time;
  time(&start_time);
  srand(time(NULL));
  
  // Initialize or clear the transposition table
  init_tt();

  // Set default move in case we don't have time for even depth 1
  MoveBranch branch;
  branch.move_count = move_possible(board, branch.moves);
  if (branch.move_count > 0) {
    *best_move = branch.moves[0];
  }

  for (int depth = 1; depth < MAX_DEPTH; depth++) {
    eval = alpha_beta(board, depth, -10000, 10000, pv, start_time, max_allowed);

    // Check if time has run out
    time_t current_time;
    time(&current_time);
    if (difftime(current_time, start_time) > max_allowed) {
      break;
    }

    // Update best move if we have a valid PV line
    if (pv[0].from != 0 || pv[0].to != 0) {
      *best_move = pv[0];
    }
  }

  return eval;
}
