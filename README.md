<div align="center">
  
# Robespierre

</div>

Robespierre is a chess engine designed to support both standard chess gameplay and, when the MENACE flag is activated, a "Menace Mode".

### Rules of menace

When you threaten an opponent's piece, you can choose to control it instead of your own. While controlling the opponent's piece, you can even capture other pieces belonging to your opponent. Pawns still move in their original direction.

### Why Robespierre

Under the rules of Menace, the bot, when its king was threatened, would prioritize capturing its own king to avoid check. The game would then continue without a king—a truly revolutionary act. (This bug has since been resolved.)

### TODO

- [ ] draw
- [x] UCI
- [ ] Search gestion
  - [x] Minimax
  - [x] Alpha-beta
  - [ ] Iterative deepening
  - [ ] Quiescence search
  - [ ] Apiration window
- [ ] eval
  - [ ] by position on board
  - [ ] threat
  - [ ] checkmate -inf
  - [ ] check
  - [ ] 0.1 of random eval (to make it more interesting)
  - [ ] passed pawn
  - [ ] pawn structure
