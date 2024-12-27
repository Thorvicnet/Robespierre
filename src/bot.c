#include "bot.h"

void change_score (int *score, Bb bb, int value, int sign){
    //Modifies the score according to the pieces in bb
    int count = 0;
    while (bb) {
        count++;
        bb &= bb - 1;
    }
    *score += count * value * sign;
}

int evaluate (Board *board){
    //Currently returns the material advantage of white

    //Values attributed to different pieces (could be modified according to the state of the game)
    int pawn_value = 10;
    int knight_value = 30;
    int bishop_value = 50;
    int rook_value = 60;
    int queen_value = 110;

    int score = 0;
    int* score_pointer = &score;

    change_score(score_pointer, board -> white_pawns, pawn_value, 1);
    change_score(score_pointer, board -> white_knights, knight_value, 1);
    change_score(score_pointer, board -> white_bishops, bishop_value, 1);
    change_score(score_pointer, board -> white_rooks, rook_value, 1);
    change_score(score_pointer, board -> white_queens, queen_value, 1);

    change_score(score_pointer, board -> black_pawns, pawn_value, -1);
    change_score(score_pointer, board -> black_knights, knight_value, -1);
    change_score(score_pointer, board -> black_bishops, bishop_value, -1);
    change_score(score_pointer, board -> black_rooks, rook_value, -1);
    change_score(score_pointer, board -> black_queens, queen_value, -1);

    return score;
}

Move choose_with_depth (Board *board, int depth){
    //Chooses the best move, according to a minimax search
    // Currently checks if the move is possible even though we know it is - kinda

    List_of_move list_moves = possible_move(board); //Should be a list of every valid move
    assert(list_moves.nb > 0); // To be dealt with later

    int best_index = 0;
    int best_eval = board->color == WHITE ? -1000 : 1000;
    Board *new_board;

    for (int i=0; i<list_moves.nb; i++){

        new_board = board_copy(board);
        move(new_board, list_moves.list[i]);

        if (depth > 0) move(new_board, choose_with_depth(new_board, depth-1));
        int eval = evaluate(new_board);

        if ((board->color == WHITE && eval > best_eval) || 
        (board->color == BLACK && eval < best_eval)){
            best_index = i;
            best_eval = eval;
        }
    }

    board_free(new_board);
    return list_moves.list[best_index];

}

Move choose (Board *board){
    //Chooses the best move according to the evaluation
    //Currently lacks : alpha-beta pruning, iterative deepening

    return choose_with_depth(board, 5); //currently arbitrary depth
}
