#include "bot.h"

typedef struct {Move mo; int value;} Vmove; //Valued move, useful for the search

void change_score (int *score, Bb bb, int value){
    //Modifies the score according to the pieces in bb
    int count = 0;
    while (bb) {
        count++;
        bb &= bb - 1;
    }
    *score += count * value;
}

int evaluate (Board *board){
    //Returns an evaluation of the position, without depth

    //Values attributed to different pieces (could be modified according to the state of the game)
    int pawn_value = 10;
    int knight_value = 30;
    int bishop_value = 50;
    int rook_value = 60;
    int queen_value = 110;

    int king_value = 5000;

    int threat_value = 2;

    if (board -> color == WHITE && ((board -> white_threat & board -> black_kings) != 0)) return king_value;
    if (board -> color == BLACK && ((board -> black_threat & board -> white_kings) != 0)) return -king_value;

    int score = 0;
    int* score_pointer = &score;

    change_score(score_pointer, board -> white_pawns, pawn_value);
    change_score(score_pointer, board -> white_knights, knight_value);
    change_score(score_pointer, board -> white_bishops, bishop_value);
    change_score(score_pointer, board -> white_rooks, rook_value);
    change_score(score_pointer, board -> white_queens, queen_value);

    change_score(score_pointer, board -> black_pawns, -pawn_value);
    change_score(score_pointer, board -> black_knights, -knight_value);
    change_score(score_pointer, board -> black_bishops, -bishop_value);
    change_score(score_pointer, board -> black_rooks, -rook_value);
    change_score(score_pointer, board -> black_queens, -queen_value);

    change_score(score_pointer, board -> white_threat, threat_value);
    change_score(score_pointer, board -> black_threat, -threat_value);

    return score;
}

Vmove choose_with_depth (Board *board, int depth, int alpha, int beta){
    // Chooses the best move, according to a minimax search with alpha-beta pruning
    // Currently checks if the move is possible even though we know it is - kinda
    // beta is greater than alpha (or else the branch is pruned)

    assert(depth>0); //Depth 0 corresponds to the static evaluation of the position - later

    List_of_move list_moves = possible_move(board); //Should be a list of every valid move
    assert(list_moves.nb > 0); // Checkmate, draw ... to be dealt with later

    int best_index = 0;
    int best_eval = board->color == WHITE ? -10000 : 10000;
    Board *new_board;

    for (int i=0; i<list_moves.nb; i++){

        new_board = board_copy(board);
        move(new_board, list_moves.list[i]);

        int eval;
        if (depth == 1) eval = evaluate(new_board);
        else eval = choose_with_depth(new_board, depth-1, alpha, beta).value;

        if ((board->color == WHITE && eval > best_eval) || 
        (board->color == BLACK && eval < best_eval)){
            best_index = i;
            best_eval = eval;
        }

        if (board->color == WHITE) alpha = alpha > eval ? alpha : eval;
        else beta = beta < eval ? beta : eval;
        if (beta <= alpha) break;

    }

    board_free(new_board);
    return (Vmove){list_moves.list[best_index], best_eval};

}

Move choose (Board *board){
    //Chooses the best move according to the evaluation
    //Currently lacks : iterative deepening

    return choose_with_depth(board, 12, -10000, 10000).mo; //currently arbitrary depth of 5
}
