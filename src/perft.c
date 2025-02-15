#include "bb.h"
#include "board.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include "uci.h"
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <wchar.h>

// Perft program, checks if the bot generate moves correctly
// TODO: start from different FEN

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const int DEPTH = 6;

long long int count_moves(Board *board, int depth) {
  if (depth == 0) {
    return 1;
  }
  Move l[MAX_MOVES];
  int moves_count = move_possible(board, l);
  Undo undo;
  long long int count = 0;
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

void run_perft_test(Board *board, char *fen, const long long int expected[],
                    int max_depth) {
  transform_board_from_fen(fen, board);
  long long int count[DEPTH];

  for (int depth = 0; depth < max_depth; depth++) {
    clock_t start = clock(), diff;
    count[depth] = count_moves(board, depth);
    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    wprintf(L"- depth %d: %d, took %ds %dms, %d clocks\n", depth, count[depth],
            msec / 1000, msec % 1000, diff);
  }

  for (int depth = 0; depth < max_depth; depth++) {
    assert(count[depth] == expected[depth]);
  }
}

int main() {
  bb_magic_init();
  Board *board = board_init();
  threat_board_update(board);

  // Starting position test
  long long int expected1[] = {1,      20,      400,       8902,
                               197281, 4865609, 119060324, 3195901860};
  run_perft_test(board,
                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                 expected1, MIN(DEPTH, 8));

  // Position 2 test
  long long int expected2[] = {6, 264, 9467, 422333, 15833292, 706045033};
  run_perft_test(
      board, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
      expected2, MIN(DEPTH, 6));

  // Position 3 test
  long long int expected3[] = {44, 1486, 62379, 2103487, 89941194};
  run_perft_test(board,
                 "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
                 expected3, MIN(DEPTH, 5));

  wprintf(L"Everything looks good\n");

  free(board->history->list_of_move);
  free(board->history);
  board_free(board);
  return 0;
}
