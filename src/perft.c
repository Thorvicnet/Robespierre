#include "bb.h"
#include "board.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <wchar.h>

const int DEPTH = 7;

int count_moves(Board *board, int depth) {
  if (depth == 0) {
    return 1;
  }
  Move l[MAX_MOVES];
  int moves_count = move_possible(board, l);
  Undo undo;
  int count = 0;
  for (int i = 0; i < moves_count; i++) {
    int res = move_make(board, &(l[i]), &undo);
    if (res) {
      continue;
    }
    count += count_moves(board, depth - 1);
    move_undo(board, &(l[i]), &undo);
  }
  return count;
}

int main() {
  bb_magic_init();
  Board *board = board_init();
  threat_board_update(board);

  int count[DEPTH];

  for (int depth = 0; depth < DEPTH; depth++) {
    clock_t start = clock(), diff;
    count[depth] = count_moves(board, depth);
    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    wprintf(L"- depth %d: %d, took %ds %dms, %d clocks\n", depth, count[depth],
            msec / 1000, msec % 1000, diff);
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
