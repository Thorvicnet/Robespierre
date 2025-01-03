#include "uci.h"

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

  return (Move){board_get(board, orig[0] + 8 * orig[1]),
                {orig[0], orig[1]},
                {dest[0], dest[1]},
                board_get(board, dest[0] + 8 * dest[1]) != EMPTY,
                promot};
}
