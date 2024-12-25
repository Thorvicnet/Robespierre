# Menace Chess Engine

<!--toc:start-->
- [Menace Chess Engine](#menace-chess-engine)
  - [Board positions](#board-positions)
  - [TODO](#todo)
<!--toc:end-->

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

- [x] debug MENACE
- [ ] promotion (long)
- [ ] draw (long)
- [ ] still incorrect castle (vachement dur pour ce que c'est) Victor
- [ ] UCI (ça passe)
- [ ] Alpha-beta (dur)
- [ ] IA génétique deep learning Nono
- [ ] eval (dur)
  - [ ] by position on board ?
  - [ ] threat
  - [ ] mate -inf
  - [ ] check

- [ ] book (avec alpha beta)
