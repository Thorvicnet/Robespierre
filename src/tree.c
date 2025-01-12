#include "tree.h"

MoveTree* create_tree(Board *board){
  //Creates a tree of board
  MoveList list_moves = move_possible(board);

  MoveTree *tree = (MoveTree*)malloc(sizeof(MoveTree));

  tree -> children_filled = false;
  
  MoveList *moves = (MoveList*)malloc(sizeof(MoveList));
  moves -> count = list_moves.count;
  moves -> capacity = list_moves.capacity;
  Move *tab_moves = (Move*)malloc(moves->capacity*sizeof(Move));
  for (int i = 0; i < moves -> count; i++) tab_moves[i] = list_moves.moves[i];
  moves -> moves = tab_moves;
  tree -> moves = moves;

  Board *b = board_copy(board);
  tree -> board = b;

  MoveTree **children = (MoveTree**)malloc(tree->moves->count*sizeof(MoveTree*));
  for (int i=0; i<tree->moves->count; i++) children[i] = (MoveTree*)malloc(sizeof(MoveTree));
  tree -> children = children;

  return tree;
}

void create_tree_children(MoveTree *tree){
  //Creates every child of tree
  int n = tree -> moves -> count;
  for (int i=0; i<n; i++){
    Board *b = board_copy(tree -> board);
    move(b, tree->moves->moves[i]);
    tree -> children[i] = create_tree(b);
    board_free(b);
  }
  tree -> children_filled = true;
}

void free_tree(MoveTree *tree){
  //Completely frees tree from memory
  if (tree -> children_filled){
    for (int i=0; i<tree->moves->count; i++) free_tree(tree -> children[i]);
  }
  free(tree -> children);
  move_list_free(tree -> moves);
  board_free(tree -> board);
  free(tree);
}

MoveTree* partially_free_tree(MoveTree *tree){
  //Frees tree from memory, apart from its first child, which is returned
  if (!(tree -> children_filled) || tree -> moves -> count == 0) exit(1);
  MoveTree *new_tree = tree -> children[0];
  for (int i=1; i<tree->moves->count; i++) free_tree(tree -> children[i]);
  free(tree -> children);
  move_list_free(tree -> moves);
  board_free(tree -> board);
  free(tree);
  return new_tree;
}

void tree_swap(MoveTree *tree, int k){
  //Swaps the positions of tree -> children[0] and tree -> children[k]
  if (k == 0) return;
  if (k >= tree -> moves -> count) exit(1);

  MoveTree *ptemp = tree -> children[0];
  tree -> children[0] = tree -> children[k];
  tree -> children[k] = ptemp;

  Move mtemp = tree -> moves -> moves[0];
  tree -> moves -> moves[0] = tree -> moves -> moves[k];
  tree -> moves -> moves[k] = mtemp;
}

int search_move_in_tree(MoveTree *tree, Move m){
  //Renvoie l'indice du move m dans tree
  //Peut potentiellement y avoir des erreurs avec les promotions
  for (int i=0; i<tree->moves->count; i++){
    Move mt = tree -> moves -> moves[i];
    if (m.orig[0]==mt.orig[0] && m.orig[1]==mt.orig[1] 
    && m.dest[0]==mt.dest[0] && m.dest[1]==mt.dest[1] 
    && m.promote==mt.promote){
      return i;
    }
  }
  wprintf(L"move not found in tree\n");
  exit(1);
}
