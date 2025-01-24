#include "tree.h"

MoveTree *create_tree(Board *board) {
  // Creates a tree of board
  MoveTree *tree = (MoveTree *)malloc(sizeof(MoveTree));
  if (tree == NULL) {
    wprintf(L"F*ck your memory\n");
    exit(EXIT_FAILURE);
  }

  tree->children_filled = false;

  Move *list_moves = (Move *)malloc(sizeof(Move) * MAX_MOVES);

  int moves_count = move_possible(board, list_moves);
  tree->moves = list_moves;
  tree->moves_count = moves_count;

  tree->children = (MoveTree **)malloc(tree->moves_count * sizeof(MoveTree *));
  if (tree->children == NULL) {
    wprintf(L"F*ck your memory\n");
    exit(EXIT_FAILURE);
  }

  return tree;
}

void create_tree_children(MoveTree *tree, Board *board) {
  if (tree->children_filled)
    exit(EXIT_FAILURE);
  int n = tree->moves_count;
  int offset = 0;
  for (int i = 0; i < n; i++) {
    Undo undo;
    int res = move_make(board, &tree->moves[i], &undo);
    if (res) {
      offset++;
      continue;
    } else {
      if (offset > 0)
        tree->moves[i - offset] = tree->moves[i];
      tree->children[i - offset] = create_tree(board);
    }
    move_undo(board, &tree->moves[i], &undo);
  }
  tree->moves_count = n - offset;
  tree->children_filled = true;
}

void free_tree(MoveTree *tree) {
  // Completely frees tree from memory
  if (tree->children_filled) {
    for (int i = 0; i < tree->moves_count; i++)
      free_tree(tree->children[i]);
  }
  if (tree->moves) {
    free(tree->moves);
  }
  free(tree->children);
  free(tree);
}

void partially_free_tree(MoveTree **tree) {
  // Frees tree from memory, apart from its first child, which is returned
  if (!(*tree)->children_filled || (*tree)->moves_count == 0)
    exit(5);
  MoveTree *temp = (*tree)->children[0];
  for (int i = 1; i < (*tree)->moves_count; i++)
    free_tree((*tree)->children[i]);
  free((*tree)->children);
  free((*tree)->moves);
  free((*tree));
  *tree = temp;
}

void tree_swap(MoveTree *tree, int k) {
  // Swaps the positions of tree -> children[0] and tree -> children[k]
  if (!tree->children_filled || k >= tree->moves_count)
    exit(1);
  if (k == 0)
    return;

  MoveTree *ptemp = tree->children[0];
  tree->children[0] = tree->children[k];
  tree->children[k] = ptemp;

  Move mtemp = tree->moves[0];
  tree->moves[0] = tree->moves[k];
  tree->moves[k] = mtemp;
}

int search_move_in_tree(MoveTree *tree, Move m) {
  // Returns the index of m in tree -> moves.moves
  for (int i = 0; i < tree->moves_count; i++) {
    Move mt = tree->moves[i];
    if (m.from == mt.from && m.to == mt.to && m.promote == mt.promote) {
      return i;
    }
  }
  wprintf(L"move not found in tree\n");
  exit(1);
}

void tree_rotation(MoveTree *tree, int k) {
  // Rotates all moves in tree->moves->moves until rank k
  // So that move k goes in first, the previous best move in second, etc.
  if (!(tree->children_filled) || k >= tree->moves_count)
    exit(1);
  for (int i = 1; i <= k; i++)
    tree_swap(tree, i);
}
