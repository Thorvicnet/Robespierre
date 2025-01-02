#ifndef HISTORY_H
#define HISTORY_H

#include "types.h"
#include <signal.h>
#include <stdlib.h>
#include <wchar.h>

Move stack_peek(Stack *stack);
Stack *stack_create(void);
void stack_push(Stack *stack, Move value);
Move stack_pop(Stack *stack);

#endif // HISTORY_H
