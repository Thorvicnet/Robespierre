#include "board.h"
#include "types.h"
#include "uci.h"
#include <string.h>
#include <wchar.h>

char* move_to_algebric(Move move){
    char alphabet[] = "hgfedcba";
    char number[] = "1345678";
    char* orig = strcat(&alphabet[move.orig[0]],&alphabet[move.orig[1]]);
    char* dest = strcat(&alphabet[move.dest[0]],&alphabet[move.dest[1]]);
    char* promot;
    if ((PIECE(move.promote)) == QUEEN) promot = "q";
    if ((PIECE(move.promote)) == BISHOP) promot = "b";
    if ((PIECE(move.promote)) == ROOK) promot = "r";
    if ((PIECE(move.promote)) == KNIGHT) promot = "k";
    else promot = "";
    char* ret = strcat(orig,dest);
    ret = strcat(ret,promot);
    return ret;
}

Move algebric_to_move(char* ch, Board *board){
    int orig[2] = {(int)7-(ch[0]-97), (int)ch[1]-49};

    int dest[2] = {(int)7-(ch[2]-97), (int)ch[3]-49};
    int promot = EMPTY;
    if (strlen(ch) > 4){
        if ( ch[4] == 'q') promot = (QUEEN|board->color);
        if ( ch[4] == 'b') promot = (BISHOP|board->color);
        if ( ch[4] == 'r') promot = (ROOK|board->color);
        if ( ch[4] == 'k') promot = (KNIGHT|board->color);
    }
    
    return (Move){board_get(board, orig[0]+8*orig[1]), {orig[0],orig[1]}, {dest[0], dest[1]}, board_get(board, dest[0] + 8*dest[1]) != EMPTY, promot};
}

