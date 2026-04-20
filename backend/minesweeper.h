#ifndef MINESWEEPER_H
#define MINESWEEPER_H

typedef struct Cell {
    int isMine;
    int isRevealed;
    int isFlagged;
    int neighborMines;
} Cell;

typedef struct Board {
    Cell **grid;
    int rows;
    int cols;
    int mines;
    int revealedCount;
    int gameOver;
} Board;

Board *create_board(int rows, int cols, int mines);
void destroy_board(Board *board);
int reveal_cell(Board *board, int r, int c);
int toggle_flag(Board *board, int r, int c);
const char *get_state_json(Board *board);

#endif
