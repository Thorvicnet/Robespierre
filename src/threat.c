#include "threat.h"

bool check_threatened (Board *board, int cell[2], int color, int depth){
  //Returns true if cell is threatened by pieces of the opposite color at the chosen depth (1 for a direct threat, 2 for an indirect one)
  //Pawns undetected yet (waiting for en passant)
  if (depth == 0) return false;
  
  int dirs[16][2] = {{-2,-1}, {-2,1}, {-1,-2}, {-1,-1}, {-1, 0}, {-1, 1}, {-1,2}, {0,-1}, {0, 1}, {1, -2}, {1,-1}, {1, 0}, {1, 1}, {1, 2}, {2,-1}, {2, 1}};
  
  for (int i=0; i<16; i++){
    int current_cell[2] = {cell[0] + dirs[i][0], cell[1] + dirs[i][1]};
    int addon_knight = abs(dirs[i][0])+abs(dirs[i][1]) == 3 ? 9 : 0; //To prevent knights from doing several jumps
    
    while (current_cell[0] >= 0 && current_cell[1] >= 0 && current_cell[0] < 8 && current_cell[1] < 8){
      int piece = board_get(board, current_cell[0] + 8 * current_cell[1]);

      if (piece == EMPTY) {
        current_cell[0] += dirs[i][0] + addon_knight;
        current_cell[1] += dirs[i][1];
      }
        
      else {
        
        if ((piece & 0xF0) == board->color) {
          int new_color = color == WHITE ? BLACK : WHITE;
          if (check_threatened(board, current_cell, new_color, depth-1)) return true;
          current_cell[0] = -1;
        }
            
        else {
          if ((piece & 0x0F) == QUEEN 
            || (abs(dirs[i][0])+abs(dirs[i][1]) == 1 && (piece & 0x0F) == BISHOP) 
            || (abs(dirs[i][0])+abs(dirs[i][1]) == 2 && (piece & 0x0F) == ROOK)
            || (abs(dirs[i][0])+abs(dirs[i][1]) == 3 && (piece & 0x0F) == KNIGHT)) return true;
          current_cell[0] = -1;
        }
      }
      
    }
  }

//Insert pawn code here
  
  return false;
  
}
