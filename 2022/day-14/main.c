// compile & run:
// part1: gcc main.c -O2 -DPART1 && ./a.out
// part2: gcc main.c -O2 -DPART2 && ./a.out

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define GRID_MAX_COL 510
#ifdef PART1
#define GRID_MAX_ROW 153
#endif
#ifdef PART2
#define GRID_MAX_ROW 153 + 2
#endif

#if 1
#define INPUT_FILE "input"
#else
#define INPUT_FILE "input_test"
#endif

typedef enum cave_tile_e {
    AIR = 0,
    ROCK,
    SAND,
    ABYSS,
} cave_tile_t;

typedef struct coord_s {
    size_t col;
    size_t row;
} coords_t;

static cave_tile_t grid[GRID_MAX_COL + 1 /* col */][GRID_MAX_ROW + 1 /* row */] = { AIR };
static coords_t sand_source = { .col = 500, .row = 0 };

void draw_rock_line(coords_t rock1, coords_t rock2) {
    if (rock1.col == rock2.col) {
        size_t min, max;
        if (rock1.row < rock2.row) {
            min = rock1.row;
            max = rock2.row;
        } else {
            min = rock2.row;
            max = rock1.row;
        }
        for (size_t i = min; i <= max; ++i) {
            (grid[rock1.col][i]) = ROCK;
        }
    } else if (rock1.row == rock2.row) {
        size_t min, max;
        if (rock1.col < rock2.col) {
            min = rock1.col;
            max = rock2.col;
        } else {
            min = rock2.col;
            max = rock1.col;
        }
        for (size_t i = min; i <= max; ++i) {
            (grid[i][rock1.row]) = ROCK;
        }
    } else {
        size_t step_col, step_y;
        if (rock1.col < rock2.col) {
            step_col = 1;
        } else {
            step_col = -1;
        }
        if (rock1.row < rock2.row) {
            step_y = 1;
        } else {
            step_y = -1;
        }

        size_t min, max;
        if (rock1.col < rock2.col) {
            min = rock1.col;
            max = rock2.col;
        } else {
            min = rock2.col;
            max = rock1.col;
        }

        for (size_t i = min; i <= max; ++i) {
            (grid[i + step_col][rock1.row + step_y]) = ROCK;
        }
    }
}

void display_grid(size_t r1, size_t r2, size_t col1, size_t col2) {
    for (size_t row = r1; row < r2; ++row) {
        for (size_t col = col1; col < col2; ++col) {
            if (col == sand_source.col && row == sand_source.row) {
                printf("+");
                continue;
            }

            switch (grid[col][row]) {
                case AIR:
                    printf(".");
                    break;
                case ROCK:
                    printf("#");
                    break;
                case SAND:
                    printf("o");
                    break;
                default:
                    break;
            }
        }
        printf("\n");
    }
}


/* Simulates the free fall of a grain of sand from sand_source coordinates.
 * Returns true if the sand grain rested peacefully, false if it reached the abyss.
 */
#ifdef PART1
bool sprinkle_sand() {
    coords_t sand_pos = sand_source;

    while (1) {
        if (sand_pos.row + 1 > GRID_MAX_ROW) {
            // printf("Reached abyss!\n");
            return false;
        }

        if (/* below tile */ grid[sand_pos.col][sand_pos.row + 1] == AIR) {
            grid[sand_pos.col][sand_pos.row] = AIR;
            sand_pos.row = sand_pos.row + 1;
            grid[sand_pos.col][sand_pos.row] = SAND;
        } else if (/* below left */ grid[sand_pos.col - 1][sand_pos.row + 1] == AIR) {
            grid[sand_pos.col][sand_pos.row] = AIR;
            sand_pos.col = sand_pos.col - 1;
            sand_pos.row = sand_pos.row + 1;
            grid[sand_pos.col][sand_pos.row] = SAND;
        } else if (/* below right */ grid[sand_pos.col + 1][sand_pos.row + 1] == AIR) {
            grid[sand_pos.col][sand_pos.row] = AIR;
            sand_pos.col = sand_pos.col + 1;
            sand_pos.row = sand_pos.row + 1;
            grid[sand_pos.col][sand_pos.row] = SAND;
        } else {
            // rest in peace
            break;
        }

    }
    return true;
}
#endif

/* Simulates the free fall of a grain of sand from sand_source coordinates.
 * Returns true if the sand grain rested peacefully, false if it
 * can make no movement in the grid */
#ifdef PART2
bool sprinkle_sand(size_t highest_row) {
    coords_t sand_pos = sand_source;

    while (1) {
        if (sand_pos.row >= highest_row) {
            // reached the bottom-most row
            break;
        }

        if (/* below tile */ grid[sand_pos.col][sand_pos.row + 1] == AIR) {
            grid[sand_pos.col][sand_pos.row] = AIR;
            sand_pos.row = sand_pos.row + 1;
            grid[sand_pos.col][sand_pos.row] = SAND;
            continue;
        } else if (/* below left */ grid[sand_pos.col - 1][sand_pos.row + 1] == AIR) {
            grid[sand_pos.col][sand_pos.row] = AIR;
            sand_pos.col = sand_pos.col - 1;
            sand_pos.row = sand_pos.row + 1;
            grid[sand_pos.col][sand_pos.row] = SAND;
            continue;
        } else if (/* below right */ grid[sand_pos.col + 1][sand_pos.row + 1] == AIR) {
            grid[sand_pos.col][sand_pos.row] = AIR;
            sand_pos.col = sand_pos.col + 1;
            sand_pos.row = sand_pos.row + 1;
            grid[sand_pos.col][sand_pos.row] = SAND;
            continue;
        }

        if (sand_pos.col == sand_source.col && sand_pos.row == sand_source.row) {
            return false;
        } else {
            break;
        }

    }
    return true;
}
#endif

int main() {
    char buf[256];

    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("Failed to open input file");
        exit(1);
    }

    grid[sand_source.col][sand_source.row] = SAND;

#ifdef PART2
    size_t highest_row = 0;
#endif

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        // strip trailing newline character
        buf[strcspn(buf, "\n")] = 0;

        if (buf[0] == '\0') {
            continue;
        }

        char* p = &buf[0];
        char* q;
        coords_t c1, c2;
        while (p != NULL && *p) {
            if (isspace(*p)) continue;

            if ((q = strchr(p, '-'))) {
                char tmp;
                sscanf(p, "%zu,%zu -> %zu,%zu%s", &c1.col, &c1.row, &c2.col, &c2.row, &tmp);

#ifdef PART2
                if (c1.row > highest_row) {
                    highest_row = c1.row;
                }
                if (c2.row > highest_row) {
                    highest_row = c2.row;
                }
#endif

                draw_rock_line(c1, c2);
                p = q + 3;
            } else {
                break;
            }
        }
    }

    int sand_counter = 0;
    while (1) {
#ifdef PART1
        if (!sprinkle_sand()) {
            break;
        }
#elif defined(PART2)
        if (!sprinkle_sand(highest_row + 2)) {
            break;
        }
#endif

        sand_counter++;
    }

#ifdef PART1
    printf("part1: %d\n", sand_counter);
#endif

#ifdef PART2
    printf("part2: %d\n", sand_counter);
#endif
    
    fclose(fp);
    return 0;
}

