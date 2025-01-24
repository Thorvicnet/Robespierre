#ifndef TREE_H
#define TREE_H

#include "board.h"
#include "move.h"
#include "types.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

MoveTree *create_tree(Board *board);
void create_tree_children(MoveTree *tree, Board *board);
void free_tree(MoveTree *tree);
void partially_free_tree(MoveTree **tree);
void tree_swap(MoveTree *tree, int k);
int search_move_in_tree(MoveTree *tree, Move m);
void tree_rotation(MoveTree *tree, int k);

#endif // TREE_H
