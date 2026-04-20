#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Cell {
    int mine;
    int open;
    int flag;
    int neighbor;
} Cell;

typedef struct Board {
    Cell *cells;
    int rows;
    int cols;
    int mines;
    int opened;
    int flags;
    int state; /* 0: playing, 1: win, 2: lose */
} Board;

static int idx(Board *b, int r, int c) { return r * b->cols + c; }
static int in_bounds(Board *b, int r, int c) { return r >= 0 && r < b->rows && c >= 0 && c < b->cols; }
static Cell *cell_at(Board *b, int r, int c) { return &b->cells[idx(b, r, c)]; }

static void place_mines(Board *b) {
    int placed = 0;
    while (placed < b->mines) {
        int r = rand() % b->rows;
        int c = rand() % b->cols;
        Cell *cell = cell_at(b, r, c);
        if (!cell->mine) {
            cell->mine = 1;
            placed++;
        }
    }
}

static void compute_neighbors(Board *b) {
    int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            Cell *cell = cell_at(b, r, c);
            if (cell->mine) {
                cell->neighbor = -1;
                continue;
            }
            int count = 0;
            for (int i = 0; i < 8; i++) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (in_bounds(b, nr, nc) && cell_at(b, nr, nc)->mine) count++;
            }
            cell->neighbor = count;
        }
    }
}

static void reveal_all_mines(Board *b) {
    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            Cell *cell = cell_at(b, r, c);
            if (cell->mine) cell->open = 1;
        }
    }
}

static void flood_open(Board *b, int sr, int sc) {
    int max = b->rows * b->cols;
    int *qr = (int *)malloc(sizeof(int) * (size_t)max);
    int *qc = (int *)malloc(sizeof(int) * (size_t)max);
    if (!qr || !qc) {
        free(qr);
        free(qc);
        return;
    }

    int head = 0, tail = 0;
    qr[tail] = sr;
    qc[tail] = sc;
    tail++;

    int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    while (head < tail) {
        int r = qr[head];
        int c = qc[head];
        head++;

        for (int i = 0; i < 8; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (!in_bounds(b, nr, nc)) continue;
            Cell *next = cell_at(b, nr, nc);
            if (next->open || next->flag || next->mine) continue;
            next->open = 1;
            b->opened++;
            if (next->neighbor == 0 && tail < max) {
                qr[tail] = nr;
                qc[tail] = nc;
                tail++;
            }
        }
    }

    free(qr);
    free(qc);
}

Board *ms_create(int rows, int cols, int mines) {
    if (rows < 5 || cols < 5 || rows > 30 || cols > 30) return NULL;
    if (mines <= 0 || mines >= rows * cols) return NULL;

    Board *b = (Board *)malloc(sizeof(Board));
    if (!b) return NULL;
    memset(b, 0, sizeof(Board));
    b->rows = rows;
    b->cols = cols;
    b->mines = mines;
    b->state = 0;
    b->cells = (Cell *)calloc((size_t)(rows * cols), sizeof(Cell));
    if (!b->cells) {
        free(b);
        return NULL;
    }

    srand((unsigned int)time(NULL));
    place_mines(b);
    compute_neighbors(b);
    return b;
}

void ms_destroy(Board *b) {
    if (!b) return;
    free(b->cells);
    free(b);
}

int ms_open(Board *b, int r, int c) {
    if (!b || b->state != 0 || !in_bounds(b, r, c)) return 0;
    Cell *cell = cell_at(b, r, c);
    if (cell->open || cell->flag) return 0;

    cell->open = 1;
    b->opened++;

    if (cell->mine) {
        b->state = 2;
        reveal_all_mines(b);
        return 2;
    }
    if (cell->neighbor == 0) flood_open(b, r, c);

    if (b->opened >= b->rows * b->cols - b->mines) {
        b->state = 1;
        return 1;
    }
    return 1;
}

int ms_toggle_flag(Board *b, int r, int c) {
    if (!b || b->state != 0 || !in_bounds(b, r, c)) return 0;
    Cell *cell = cell_at(b, r, c);
    if (cell->open) return 0;
    if (cell->flag) {
        cell->flag = 0;
        b->flags--;
    } else {
        cell->flag = 1;
        b->flags++;
    }
    return 1;
}

/* JSON format:
{
  "rows":9,"cols":9,"state":"playing","mines":10,"flags":2,
  "board":[[...]]
}
Hidden cell: {"open":0,"flag":0}
Open cell: {"open":1,"mine":0/1,"n":neighbor}
*/
const char *ms_state_json(Board *b) {
    static char *buf = NULL;
    static size_t cap = 0;
    if (!b) return "{\"error\":\"null board\"}";

    size_t need = (size_t)b->rows * (size_t)b->cols * 80 + 4096;
    if (need > cap) {
        char *next = (char *)realloc(buf, need);
        if (!next) return "{\"error\":\"alloc failed\"}";
        buf = next;
        cap = need;
    }

    const char *state_text = b->state == 0 ? "playing" : (b->state == 1 ? "win" : "lose");
    int n = snprintf(buf, cap, "{\"rows\":%d,\"cols\":%d,\"state\":\"%s\",\"mines\":%d,\"flags\":%d,\"board\":[",
                     b->rows, b->cols, state_text, b->mines, b->flags);
    if (n < 0) return "{\"error\":\"format failed\"}";
    size_t pos = (size_t)n;

    for (int r = 0; r < b->rows; r++) {
        n = snprintf(buf + pos, cap - pos, "%s[", r == 0 ? "" : ",");
        if (n < 0) return "{\"error\":\"format failed\"}";
        pos += (size_t)n;
        for (int c = 0; c < b->cols; c++) {
            Cell *cell = cell_at(b, r, c);
            if (cell->open) {
                n = snprintf(buf + pos, cap - pos, "%s{\"open\":1,\"flag\":%d,\"mine\":%d,\"n\":%d}",
                             c == 0 ? "" : ",", cell->flag, cell->mine, cell->neighbor < 0 ? 0 : cell->neighbor);
            } else {
                n = snprintf(buf + pos, cap - pos, "%s{\"open\":0,\"flag\":%d}",
                             c == 0 ? "" : ",", cell->flag);
            }
            if (n < 0) return "{\"error\":\"format failed\"}";
            pos += (size_t)n;
        }
        n = snprintf(buf + pos, cap - pos, "]");
        if (n < 0) return "{\"error\":\"format failed\"}";
        pos += (size_t)n;
    }

    snprintf(buf + pos, cap - pos, "]}");
    return buf;
}
