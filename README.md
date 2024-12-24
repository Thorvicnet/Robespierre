# Menace Chess Engine

## Board positions

The top line is for WHITE at the start. (board->squares[0] == WHITE_ROOK)\
If the functions takes a tuple as a position, pos\[0] is the column and pos\[1] the lines.\
If the functions just takes a pos then pos = col + line * 8 (to get the correct index in the 1D board).

♜♞♝♛♚♝♞♜\
♟♟♟♟♟♟♟♟\
\
\
\
♙♙♙♙♙♙♙♙\
♖♘♗♕♔♗♘♖

## TODO

- [] debug MENACE Victor
- [] promotion (long)
- [] draw (long)
- [] still incorrect castle (vachement dur pour ce que c'est)
- [] UCI (ça passe)
- [] Alpha-beta (dur)
- [] eval (dur)
  - [] by position on board ?
  - [] threat
  - [] mate -inf
  - [] check

- [] book (avec alpha beta)
