#include "threat.h"

bool check_threatened (Board *board, int cell[2], int color, int depth){
  //Returns true if cell is threatened by pieces of the opposite color at the chosen depth (1 for a direct threat, 2 for an indirect one)
  //En passant undetected yet
  if (depth == 0) return false;
  
  int dirs[16][2] = {{-2,-1}, {-2,1}, {-1,-2}, {-1,-1}, {-1, 0}, {-1, 1}, {-1,2}, {0,-1}, {0, 1}, {1, -2}, {1,-1}, {1, 0}, {1, 1}, {1, 2}, {2,-1}, {2, 1}};
  
  for (int i=0; i<16; i++){
    int current_cell[2] = {cell[0] + dirs[i][0], cell[1] + dirs[i][1]};
    int dist = abs(dirs[i][0])+abs(dirs[i][1]);
    int addon_knight = dist == 3 ? 9 : 0; //To prevent knights from doing several jumps
    
    while (current_cell[0] >= 0 && current_cell[1] >= 0 && current_cell[0] < 8 && current_cell[1] < 8){
      int piece = board_get(board, current_cell[0] + 8 * current_cell[1]);

      if (piece == EMPTY) {
        current_cell[0] += dirs[i][0] + addon_knight;
        current_cell[1] += dirs[i][1];
      }
        
      else {
        
        if ((piece & 0xF0) == board->color) {
          if ((dist <= 2 && (piece & 0x0F) == QUEEN)
            || (dist == 1 && (piece & 0x0F) == BISHOP)
            || (dist == 2 && (piece & 0x0F) == ROOK)
            || (dist == 3 && (piece & 0x0F) == KNIGHT)){
            if (check_threatened(board, current_cell, color, depth-1)) return true;
          }
          current_cell[0] = -1;
        }
            
        else {
          if ((dist <= 2 && (piece & 0x0F) == QUEEN)
            || (dist == 1 && (piece & 0x0F) == BISHOP)
            || (dist == 2 && (piece & 0x0F) == ROOK)
            || (dist == 3 && (piece & 0x0F) == KNIGHT)) return true;
          
          current_cell[0] = -1;
        }
      }
      
    }
  }

  //Insert pawn code here

  if (cell[1] < 6){
    
    if (cell[0] > 0){
      int piece = board_get(board, cell[0]-1 + 8*(cell[1]+1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == BLACK){
        if (color == WHITE) return true;
        if (check_threatened(board, (int[]){cell[0]-1, cell[1]+1}, color, depth-1) return true;
      }
    }
    if (cell[0] < 7){
      int piece = board_get(board, cell[0]+1 + 8*(cell[1]+1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == BLACK){
        if (color == WHITE) return true;
        if (check_threatened(board, (int[]){cell[0]+1, cell[1]+1}, color, depth-1) return true;
      }
    }
    
  }

  if (cell[1] > 1){
    
    if (cell[0] > 0){
      int piece = board_get(board, cell[0]-1 + 8*(cell[1]-1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == WHITE){
        if (color == BLACK) return true;
        if (check_threatened(board, (int[]){cell[0]-1, cell[1]-1}, color, depth-1) return true;
      }
    }
    if (cell[0] < 7){
      int piece = board_get(board, cell[0]+1 + 8*(cell[1]-1));
      if ((piece & 0x0F) == PAWN && (piece & 0xF0) == WHITE){
        if (color == BLACK) return true;
        if (check_threatened(board, (int[]){cell[0]+1, cell[1]-1}, color, depth-1) return true;
      }
    }
    
  }

  return false;
  
}
