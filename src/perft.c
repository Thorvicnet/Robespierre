#include "bb.h"
#include "board.h"
#include "move.h"
#include "threat.h"
#include "types.h"
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <wchar.h>
#define DEPTH 7
#define NUM_THREADS 8

typedef struct {
  Board *board;
  Move move; // Single move instead of array
  int depth;
  int *result;
  pthread_mutex_t *mutex;
} ThreadArgs;

int count_moves(Board *board, int depth);

void *thread_count_moves(void *arg) {
  ThreadArgs *args = (ThreadArgs *)arg;
  Board *board_cpy = board_copy(args->board);
  Undo undo;
  int local_count = 0;

  int res = move_make(board_cpy, &(args->move), &undo);
  if (!res) {
    local_count = count_moves(board_cpy, args->depth - 1);
    move_undo(board_cpy, &(args->move), &undo);
  }

  pthread_mutex_lock(args->mutex);
  *args->result += local_count;
  pthread_mutex_unlock(args->mutex);

  board_free(board_cpy);
  return NULL;
}

int parallel_count_moves(Board *board, int depth) {
  if (depth <= 1) {
    return count_moves(board, depth);
  }

  Move moves[MAX_MOVES];
  int moves_count = move_possible(board, moves);
  int count = 0;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  // Create threads based on actual move count
  int num_threads = moves_count < NUM_THREADS ? moves_count : NUM_THREADS;
  pthread_t *threads = malloc(moves_count * sizeof(pthread_t));
  ThreadArgs *args = malloc(moves_count * sizeof(ThreadArgs));

  // Launch one thread per move
  for (int i = 0; i < moves_count; i++) {
    args[i].board = board;
    args[i].move = moves[i];
    args[i].depth = depth;
    args[i].result = &count;
    args[i].mutex = &mutex;
    pthread_create(&threads[i], NULL, thread_count_moves, &args[i]);
  }

  // Join all threads
  for (int i = 0; i < moves_count; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  free(threads);
  free(args);
  return count;
}

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
  struct timeval start, end;

  for (int depth = 0; depth < DEPTH; depth++) {
    gettimeofday(&start, NULL);
    count[depth] = parallel_count_moves(board, depth);
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    wprintf(L"- depth %d: %d, took %.3fs\n", depth, count[depth], elapsed);
  }

  assert(count[0] == 1);
  assert(count[1] == 20);
  assert(count[2] == 400);
  assert(count[3] == 8902);
  assert(count[4] == 197281);
  assert(count[5] == 4865609);
  wprintf(L"Everything looks good\n");

  free(board->history->list_of_move);
  free(board->history);
  board_free(board);
  return 0;
}
