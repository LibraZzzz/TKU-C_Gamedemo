#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "minesweeper.h"
#include "queue.h"

static int in_bounds(Board *b, int r, int c) {
    return b && r >= 0 && r < b->rows && c >= 0 && c < b->cols;
}

static void compute_neighbors(Board *b) {
    int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            if (b->grid[r][c].isMine) {
                b->grid[r][c].neighborMines = -1;
                continue;
            }
            int count = 0;
            for (int i = 0; i < 8; i++) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (in_bounds(b, nr, nc) && b->grid[nr][nc].isMine) count++;
            }
            b->grid[r][c].neighborMines = count;
        }
    }
}

static void place_mines(Board *b) {
    int placed = 0;
    while (placed < b->mines) {
        int r = rand() % b->rows;
        int c = rand() % b->cols;
        if (!b->grid[r][c].isMine) {
            b->grid[r][c].isMine = 1;
            placed++;
        }
    }
}

static void reveal_all_mines(Board *b) {
    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            if (b->grid[r][c].isMine) b->grid[r][c].isRevealed = 1;
        }
    }
}

static void flood_fill(Board *b, int sr, int sc) {
    int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    Queue *q = queue_create();
    if (!q) return;
    queue_push(q, sr, sc);

    while (!queue_is_empty(q)) {
        int r, c;
        if (!queue_pop(q, &r, &c)) break;

        for (int i = 0; i < 8; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (!in_bounds(b, nr, nc)) continue;

            Cell *cell = &b->grid[nr][nc];
            if (cell->isRevealed || cell->isFlagged || cell->isMine) continue;

            cell->isRevealed = 1;
            b->revealedCount++;
            if (cell->neighborMines == 0) queue_push(q, nr, nc);
        }
    }

    queue_destroy(q);
}

Board *create_board(int rows, int cols, int mines) {
    if (rows < 5 || cols < 5 || rows > 30 || cols > 30) return NULL;
    if (mines <= 0 || mines >= rows * cols) return NULL;

    Board *b = (Board *)malloc(sizeof(Board));
    if (!b) return NULL;

    b->rows = rows;
    b->cols = cols;
    b->mines = mines;
    b->revealedCount = 0;
    b->gameOver = 0;
    b->grid = (Cell **)malloc(sizeof(Cell *) * rows);
    if (!b->grid) {
        free(b);
        return NULL;
    }

    for (int r = 0; r < rows; r++) {
        b->grid[r] = (Cell *)calloc((size_t)cols, sizeof(Cell));
        if (!b->grid[r]) {
            for (int i = 0; i < r; i++) free(b->grid[i]);
            free(b->grid);
            free(b);
            return NULL;
        }
    }

    srand((unsigned int)time(NULL));
    place_mines(b);
    compute_neighbors(b);
    return b;
}

void destroy_board(Board *b) {
    if (!b) return;
    for (int r = 0; r < b->rows; r++) {
        free(b->grid[r]);
    }
    free(b->grid);
    free(b);
}

int reveal_cell(Board *b, int r, int c) {
    if (!in_bounds(b, r, c) || b->gameOver) return 0;

    Cell *cell = &b->grid[r][c];
    if (cell->isRevealed || cell->isFlagged) return 0;

    cell->isRevealed = 1;
    b->revealedCount++;

    if (cell->isMine) {
        b->gameOver = 1;
        reveal_all_mines(b);
        return 2;
    }

    if (cell->neighborMines == 0) {
        flood_fill(b, r, c);
    }

    if (b->revealedCount == (b->rows * b->cols - b->mines)) {
        b->gameOver = 2;
        return 1;
    }

    return 1;
}

int toggle_flag(Board *b, int r, int c) {
    if (!in_bounds(b, r, c) || b->gameOver) return 0;
    Cell *cell = &b->grid[r][c];
    if (cell->isRevealed) return 0;
    cell->isFlagged = !cell->isFlagged;
    return 1;
}

const char *get_state_json(Board *b) {
    static char *buffer = NULL;
    static size_t buffer_size = 0;

    if (!b) return "{\"error\":\"board is null\"}";

    size_t estimated = (size_t)b->rows * (size_t)b->cols * 64 + 1024;
    if (estimated > buffer_size) {
        char *next = (char *)realloc(buffer, estimated);
        if (!next) return "{\"error\":\"alloc failed\"}";
        buffer = next;
        buffer_size = estimated;
    }

    int n = snprintf(buffer, buffer_size,
        "{\"rows\":%d,\"cols\":%d,\"gameOver\":%d,\"cells\":[",
        b->rows, b->cols, b->gameOver);
    if (n < 0) return "{\"error\":\"format failed\"}";
    size_t pos = (size_t)n;

    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            Cell *cell = &b->grid[r][c];
            n = snprintf(buffer + pos, buffer_size - pos,
                "%s{\"r\":%d,\"c\":%d,\"revealed\":%d,\"flagged\":%d,\"mine\":%d,\"n\":%d}",
                (r == 0 && c == 0) ? "" : ",",
                r, c,
                cell->isRevealed,
                cell->isFlagged,
                cell->isMine,
                cell->neighborMines < 0 ? 0 : cell->neighborMines);
            if (n < 0) return "{\"error\":\"format failed\"}";
            pos += (size_t)n;
        }
    }

    snprintf(buffer + pos, buffer_size - pos, "]}");
    return buffer;
}
