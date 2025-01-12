#include "uci.h"
#include "board.h"
#include "types.h"
#include <string.h>
#include <wchar.h>

char *move_to_algebric(Move move) {
  char alphabet[] = "hgfedcba";
  char number[] = "12345678";

  char *ret = (char *)malloc(6 * sizeof(char));
  if (ret == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }

  snprintf(ret, 3, "%c%c", alphabet[move.from & 7], number[move.from >> 3]);
  snprintf(ret + 2, 3, "%c%c", alphabet[move.to & 7], number[move.to >> 3]);

  if ((PIECE(move.promote)) == QUEEN)
    strcat(ret, "q");
  else if ((PIECE(move.promote)) == BISHOP)
    strcat(ret, "b");
  else if ((PIECE(move.promote)) == ROOK)
    strcat(ret, "r");
  else if ((PIECE(move.promote)) == KNIGHT)
    strcat(ret, "k");

  return ret;
}

Move algebric_to_move(char *ch, Board *board) {
  int orig[2] = {(int)7 - (ch[0] - 97), (int)ch[1] - 49};

  int dest[2] = {(int)7 - (ch[2] - 97), (int)ch[3] - 49};
  int promot = EMPTY;
  if (strlen(ch) > 4) {
    if (ch[4] == 'q')
      promot = (QUEEN | board->color);
    if (ch[4] == 'b')
      promot = (BISHOP | board->color);
    if (ch[4] == 'r')
      promot = (ROOK | board->color);
    if (ch[4] == 'k')
      promot = (KNIGHT | board->color);
  }

  return (Move){board_get(board, orig[0] + 8 * orig[1]), orig[0] + orig[1] * 8,
                dest[0] + dest[1] * 8,
                board_get(board, dest[0] + 8 * dest[1]) != EMPTY, promot};
}


int transform_board_from_fen(char* fen, Board *board){
  board_empty(board);
  int i = 0;
  int j = 0;
  for (; i < 64;j++){
    char c = fen[j];
    wprintf(L"%c", c);
    if (c == '1') i+=1;
    if (c == '2') i+=2;
    if (c == '3') i+=3;
    if (c == '4') i+=4;
    if (c == '5') i+=5;
    if (c == '6') i+=6;
    if (c == '7') i+=7;
    if (c == '8') i+=8;
    if (c == 'n') board_set(board, 63-i++, BLACK_KNIGHT);
    if (c == 'N') board_set(board, 63-i++, WHITE_KNIGHT);
    if (c == 'p') board_set(board, 63-i++, BLACK_PAWN);
    if (c == 'P') board_set(board, 63-i++, WHITE_PAWN);
    if (c == 'r') board_set(board, 63-i++, BLACK_ROOK);
    if (c == 'R') board_set(board, 63-i++, WHITE_ROOK);
    if (c == 'b') board_set(board, 63-i++, BLACK_BISHOP);
    if (c == 'B') board_set(board, 63-i++, WHITE_BISHOP);
    if (c == 'q') board_set(board, 63-i++, BLACK_QUEEN);
    if (c == 'Q') board_set(board, 63-i++, WHITE_QUEEN);
    if (c == 'k') board_set(board, 63-i++, BLACK_KING);
    if (c == 'K') board_set(board, 63-i++, WHITE_KING);
  }
  j+=2;
  if (fen[j] == 'b') {
    board->color = BLACK;
    return BLACK;
  }
  return WHITE;
}

char* create_fen_from_board(Board *board){
  char* fen = (char*)malloc(sizeof(char) * 100);

  int empty_case = 0;
  int p = 0;
  for (int i = 7; i >= 0; i--){
    for (int j = 7;j >= 0; j--){
      int piece = board_get(board, j + i * 8);
      wprintf(L"%s %d\n", fen, piece);
      if (piece == EMPTY){
        empty_case += 1;
      } else {
        if (empty_case != 0){
          fen[p] = (char)(empty_case + 48);
          wprintf(L"%c %d aaaaaaaaaaa\n", fen[p], empty_case);
          p+=1;
          empty_case = 0;
        }
        if (piece == BLACK_KNIGHT)  fen[p++] = 'n';
        else if (piece == WHITE_KNIGHT)  fen[p++] = 'N';
        else if (piece == BLACK_ROOK)  fen[p++] = 'r';
        else if (piece == WHITE_ROOK)  fen[p++] = 'R';
        else if (piece == BLACK_BISHOP)  fen[p++] = 'b';
        else if (piece == WHITE_BISHOP)  fen[p++] = 'B';
        else if (piece == BLACK_PAWN)  fen[p++] = 'p';
        else if (piece == WHITE_PAWN)  fen[p++] = 'P';
        else if (piece == BLACK_QUEEN)  fen[p++] = 'q';
        else if (piece == WHITE_QUEEN)  fen[p++] = 'Q';
        else if (piece == BLACK_KING)  fen[p++] = 'k';
        else if (piece == WHITE_KING)  fen[p++] = 'K';
      }

    }
    if (empty_case != 0){
      fen[p] = (char)(empty_case + 48);
      wprintf(L"%c %d aaaaaaaaaaa\n", empty_case+48, empty_case);
      p+=1;
      empty_case = 0;
    }
    if (i != 0) fen[p++] = '/';

  }
  fen[p++] = ' ';
  if (board->color == BLACK) fen[p++] = 'b';
  if (board->color == WHITE) fen[p++] = 'w';
  for (; p < 100; p++) fen[p] = ' ';
  return fen;
}