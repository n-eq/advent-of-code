#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BEAMS 1000
#ifdef TEST
#define INPUT "input_test"
#define GRID_SIZE 10
#else
#define GRID_SIZE 110
#define INPUT "input"
#endif

#define VALID_COORDS(row, col) (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE)

typedef struct pos_s {
    int row;
    int col;
} pos_t;

typedef enum direction_e {
    NORTH = 0,
    EAST,
    SOUTH,
    WEST,
} direction_t;

typedef enum direction_changer_e {
    VERT_SPLITTER = '|',
    HORZ_SPLITTER = '-',
    MIRROR = '/',
    ANTI_MIRROR = '\\',
} direction_changer_t;

typedef struct beam_s {
    direction_t dir;
    int row;
    int col;
} beam_t;

char grid[GRID_SIZE][GRID_SIZE];
// 4 is for the 4 directions
bool seen[GRID_SIZE][GRID_SIZE][4] = { 0 };

static void display_grid() {
    char mod[GRID_SIZE][GRID_SIZE] = { 0 };
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            mod[i][j] = grid[i][j];
            if (seen[i][j][NORTH] || seen[i][j][SOUTH] || seen[i][j][EAST] || seen[i][j][WEST]) {
                mod[i][j] = '#';
            }
        }
    }

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            printf("%c", mod[i][j]);
        }
        printf("\n");
    }
}

static int energized_count() {
    int res = 0;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (seen[i][j][NORTH] || seen[i][j][SOUTH] || seen[i][j][EAST] || seen[i][j][WEST]) {
                res++;
            }
        }
    }
    return res;
}

// get the next beam(s) based on the current beam and the position it's currently on
beam_t* get_next(beam_t beam, char dir_changer, int* nb_beams) {
    beam_t* next = malloc(sizeof(beam_t));
    *next = beam;

    *nb_beams = 1;

    int next_row = beam.row;
    int next_col = beam.col;

    // we start by determining (naively) the next coordinates
    switch (beam.dir) {
    case NORTH:
        next_row--;
        break;
    case EAST:
        next_col++;
        break;
    case SOUTH:
        next_row++;
        break;
    case WEST:
        next_col--;
        break;
    }

    // then, based on the encountered tile,
    // we determine the next direction (if we encounter a mirror),
    // or the next coordinates (if we encounter a splitter)
    switch (dir_changer) {
    case '.':
        // no direction change, but we move on
        next->row = next_row;
        next->col = next_col;
        break;
    case VERT_SPLITTER:
        switch (beam.dir) {
        case NORTH:
            next->row--;
            break;
        case SOUTH:
            next->row++;
            break;
        case EAST:
        case WEST:
            next = realloc(next, 2 * sizeof(beam_t));
            *next = beam;
            next->dir = NORTH;
            next->row = beam.row - 1; // heading up

            next[1] = beam;
            next[1].dir = SOUTH;
            next[1].row = beam.row + 1; // heading down

            *nb_beams = 2;
            break;
        }
        break;
    case HORZ_SPLITTER:
        switch (beam.dir) {
        case EAST:
            next->col++;
            break;
        case WEST:
            next->col--;
            break;
        case NORTH:
        case SOUTH:
            next = realloc(next, 2 * sizeof(beam_t));
            *next = beam;
            next->dir = WEST;
            next->col = beam.col - 1;

            next[1] = beam;
            next[1].dir = EAST;
            next[1].col = beam.col + 1;

            *nb_beams = 2;
            break;
        }
        break;
    case MIRROR:  // '/'
        switch (beam.dir) {
        case NORTH:
            next->dir = EAST;
            next->col++;
            break;
        case SOUTH:
            next->dir = WEST;
            next->col--;
            break;
        case EAST:
            next->dir = NORTH;
            next->row--;
            break;
        case WEST:
            next->dir = SOUTH;
            next->row++;
            break;
        }
        break;
    case ANTI_MIRROR: // '\'
        switch (beam.dir) {
        case NORTH:
            next->dir = WEST;
            next->col--;
            break;
        case SOUTH:
            next->dir = EAST;
            next->col++;
            break;
        case EAST:
            next->dir = SOUTH;
            next->row++;
            break;
        case WEST:
            next->dir = NORTH;
            next->row--;
            break;
        }
        break;
    }

    return next;
}

void explore(beam_t beam, bool seen[GRID_SIZE][GRID_SIZE][4]) {
    if (seen[beam.row][beam.col][beam.dir]) {
        // we've already seen this beam going this direction
        return;
    }

    if (!VALID_COORDS(beam.row, beam.col)) {
        return;
    }

    seen[beam.row][beam.col][beam.dir] = true;

    int nb_beams = 0;
    beam_t* next = get_next(beam, grid[beam.row][beam.col], &nb_beams);

    for (int i = 0; i < nb_beams; ++i) {
        explore(next[i], seen);
    }

    free(next);
}

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    char line[1000];
    int lines = 0;
    while (fgets(line, sizeof(line), f)) {
        memcpy(grid[lines++], line, GRID_SIZE);
    }

    int res = 0;
    beam_t start = {
        .dir = EAST,
        .row = 0,
        .col = 0,
    };

    explore(start, seen);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (seen[i][j][NORTH] || seen[i][j][SOUTH] || seen[i][j][EAST] || seen[i][j][WEST]) {
                res++;
            }
        }
    }
    printf("res part1: %d\n", res);

    // PART 2
    beam_t left = {
        .dir = EAST,
        .col = 0,
    };
    beam_t up = {
        .dir = SOUTH,
        .row = 0,
    };
    beam_t right = {
        .dir = WEST,
        .col = GRID_SIZE - 1,
    };
    beam_t down = {
        .dir = NORTH,
        .row = GRID_SIZE - 1,
    };

    // reinitialize the grid after each loop
    char grid_bkp[GRID_SIZE][GRID_SIZE];

    res = 0;
    // up to bottom
    for (int i = 0; i < GRID_SIZE; ++i) {
        memset(seen, 0, sizeof(seen));
        memcpy(grid, grid_bkp, sizeof(grid));

        up.col = i;
        explore(up, seen);

        int count = energized_count();
        if (count > res) {
            res = count;
        }
    }

    // bottom to top
    for (int i = 0; i < GRID_SIZE; ++i) {
        memcpy(grid_bkp, grid, sizeof(grid));
        memset(seen, 0, sizeof(seen));

        down.col = i;
        explore(down, seen);

        int count = energized_count();
        if (count > res) {
            res = count;
        }
    }

    // left to right
    for (int i = 0; i < GRID_SIZE; ++i) {
        memset(seen, 0, sizeof(seen));
        memcpy(grid, grid_bkp, sizeof(grid));

        left.row = i;
        explore(left, seen);

        int count = energized_count();
        if (count > res) {
            res = count;
        }
    }

    // right to left
    for (int i = 0; i < GRID_SIZE; ++i) {
        memset(seen, 0, sizeof(seen));
        memcpy(grid, grid_bkp, sizeof(grid));

        right.row = i;
        explore(right, seen);

        int count = energized_count();
        if (count > res) {
            res = count;
        }
    }

    printf("res part2: %d\n", res);

    fclose(f);
}
