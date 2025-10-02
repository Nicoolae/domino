# Linear Domino Game

## Overview
This project is a C99 implementation of **Linear Domino**, a game based on arranging domino tiles in order to maximize the final score.  
The program supports both **interactive mode** (human player input) and **AI mode**, where the computer automatically determines a strategy for playing.  

It was developed as part of an academic assignment, with the objective of implementing the game mechanics, an AI strategy, and possible extensions.

---

## Features
- **Interactive Mode**:  
  The player is asked to choose moves. The program validates legality, updates the board, and calculates the score until no further moves are possible.

- **AI Mode**:  
  The program computes moves automatically. The move is the one that allow the best score among the remaining tiles and is done using recursion.

- **Special Matches and Variants**:  
  - Special tiles such as:
    - `[0|0]`, can be attached everywhere.
    - `[11|11]`, sums 1 to all the tiles on the table. The tile 6 become 1.
    - `[12|21]`, copy in a mirror like the adjacent tile.  
  - Support for **2D Domino extension** (tiles can grow vertically at the ends of the board).  
  - Experimental game variations.
---

## Rules Summary
1. The game uses 21 tiles with pairs from `[1|1]` to `[6|6]`.  
2. Each player receives `N` tiles (possibly with duplicates).  
3. Tiles must be placed in a horizontal line (and optionally vertically in the 2D extension).  
4. Adjacent tiles must share a common number.  
5. The score is the sum of all numbers on the placed tiles.  
6. The game ends when no further tiles can be placed.

---

## Usage
### Interactive Mode
Run without arguments:
```bash
./domino
```
You will be prompted to enter moves. The board and score update after each move.

### AI Mode
Run with AI enabled:
```bash
./domino --ai
```
The program automatically shows the best end game sequence which is the one with the highest score.

---

## Documentation
Function, type, and file documentation is automatically generated with **Doxygen**.  
To build documentation:

```bash
doxygen Doxyfile
```

---
