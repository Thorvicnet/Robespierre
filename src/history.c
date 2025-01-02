#include "history.h"

const int MAX_MOVES = 200;

Stack *stack_create(void) {
  Stack *history = (Stack *)malloc(sizeof(Stack));
  if (history == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }
  history->list_of_move = (Move *)malloc(sizeof(Move) * MAX_MOVES);
  if (history->list_of_move == NULL) {
    wprintf(L"Your memory is doomed...\n");
    exit(EXIT_FAILURE);
  }
  history->last_move = 0;
  return history;
}

void stack_push(Stack *stack, Move value) {
  stack->list_of_move[stack->last_move++] = value;
  if (stack->last_move >= 500) {
    raise(5);
  }
}

Move stack_pop(Stack *stack) {
  if (stack->last_move <= 0) {
    raise(5);
  }
  return (stack->list_of_move[--stack->last_move]);
}

Move stack_peek(Stack *stack) {
  if (stack->last_move <= 0) {
    raise(5);
  }
  return (stack->list_of_move[stack->last_move - 1]);
}
