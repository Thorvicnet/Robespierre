# Menace Chess Engine

## Board positions
The top line is for WHITE at the start. (board->squares[0] == WHITE_ROOK)\
If the functions takes a tuple as a position pos\[0] is the column and pos\[1] the lines.\
If the functions just takes a pos then it pos = col + line * 8 (to get the correct index in the 1D board).

♜♞♝♛♚♝♞♜\
♟♟♟♟♟♟♟♟\
\
\
\
♙♙♙♙♙♙♙♙\
♖♘♗♕♔♗♘♖
