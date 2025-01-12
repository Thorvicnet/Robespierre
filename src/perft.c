#include "bb.h"
#include "board.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include <assert.h>
#include <stdbool.h>
#include <wchar.h>

const int DEPTH = 6;

int count_moves(Board *board, int depth) {
  if (depth == 0) {
    return 1;
  }
  MoveList l = move_possible(board);
  Undo undo;
  int count = 0;
  for (int i = 0; i < l.count; i++) {
    int res = move_make(board, &(l.moves[i]), &undo);
    if (res) {
      continue;
    }
    count += count_moves(board, depth - 1);
    move_undo(board, &(l.moves[i]), &undo);
  }
  return count;
}

int main() {
  bb_magic_init();
  Board *board = board_init();
  threat_board_update(board);

  int count[DEPTH];

  for (int depth = 0; depth < DEPTH; depth++) {
    count[depth] = count_moves(board, depth);
    wprintf(L"- depth %d: %d\n", depth, count[depth]);
  }

  assert(count[0] == 1);
  assert(count[1] == 20);
  assert(count[2] == 400);
  assert(count[3] == 8902);
  assert(count[4] == 197281);
  assert(count[5] == 4865609);
  wprintf(L"Everything looks good\n");
  return 0;
}
