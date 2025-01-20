#include "tree.h"

MoveTree* create_tree(Board *board){
  //Creates a tree of board
  MoveTree *tree = (MoveTree*)malloc(sizeof(MoveTree));
  if (tree == NULL){
    wprintf(L"F*ck your memory\n");
    exit(EXIT_FAILURE);
  }

  tree -> children_filled = false;

  MoveList list_moves = move_possible(board);
  list_moves.capacity = list_moves.count;
  tree -> moves = list_moves;

  tree -> board = *board;

  tree -> children = (MoveTree**)malloc(tree->moves.capacity*sizeof(MoveTree*));
  if (tree -> children == NULL){
    wprintf(L"F*ck your memory\n");
    exit(EXIT_FAILURE);
  }

  return tree;
}

void create_tree_children(MoveTree *tree){
  //Creates every child of tree
  if (tree->children_filled) exit(EXIT_FAILURE);
  int n = tree -> moves.count;
  int offset = 0;
  for (int i=0; i<n; i++){
    Board *b = board_copy(&tree -> board);
    int res = move(b, tree->moves.moves[i]);
    if (res){
      offset++;
    }else{
      if (offset > 0) tree -> moves.moves[i-offset] = tree -> moves.moves[i];
      tree -> children[i-offset] = create_tree(b);
    }
    board_free(b);
  }
  tree -> moves.count = n - offset;
  tree -> children_filled = true;
}

void free_tree(MoveTree *tree){
  //Completely frees tree from memory
  if(tree->children_filled){
    for (int i=0; i<tree->moves.count; i++) free_tree(tree -> children[i]);
    free(tree -> children);
    free(tree -> moves.moves);
  }
  free(tree);
}

MoveTree* partially_free_tree(MoveTree *tree){
  //Frees tree from memory, apart from its first child, which is returned
  if (!tree -> children_filled || tree -> moves.count == 0) exit(1);
  MoveTree *new_tree = tree -> children[0];
  for (int i=1; i<tree->moves.count; i++) free_tree(tree -> children[i]);
  free(tree -> children);
  free(tree -> moves.moves);
  free(tree);
  return new_tree;
}

void tree_swap(MoveTree *tree, int k){
  //Swaps the positions of tree -> children[0] and tree -> children[k]
  if (!tree -> children_filled || k >= tree -> moves.count) exit(1);
  if (k == 0) return;

  MoveTree *ptemp = tree -> children[0];
  tree -> children[0] = tree -> children[k];
  tree -> children[k] = ptemp;

  Move mtemp = tree -> moves.moves[0];
  tree -> moves.moves[0] = tree -> moves.moves[k];
  tree -> moves.moves[k] = mtemp;
}

int search_move_in_tree(MoveTree *tree, Move m){
  //Returns the index of m in tree -> moves.moves
  for (int i=0; i<tree->moves.count; i++){
    Move mt = tree -> moves.moves[i];
    if (m.orig[0]==mt.orig[0] && m.orig[1]==mt.orig[1] 
    && m.dest[0]==mt.dest[0] && m.dest[1]==mt.dest[1] 
    && m.promote==mt.promote){
      return i;
    }
  }
  wprintf(L"move not found in tree\n");
  exit(1);
}

void tree_rotation(MoveTree *tree, int k){
  //Rotates all moves in tree->moves->moves until rank k
  //So that move k goes in first, the previous best move in second, etc.
  if (!(tree -> children_filled) || k >= tree -> moves.count) exit(1);
  for (int i=1; i<=k; i++) tree_swap(tree, i);
}
