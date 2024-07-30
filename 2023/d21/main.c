/* Quite straighforward for part 1 (BFS), but impossible to brute force for part2.
 * Had to look for a hint. Basically the number of steps required for part2 is
 * not a random number, but represents the distance S can go straight if 202300 other grids
 * were side-by-side.
 * The assumption was to consider the number of steps a quadratic function: f(n) = an^2 + bn + c.
 * f(0) is the number of reachable positions if 65 steps are taken,
 * f(1) is the number of reachable positions if (131 + 65) steps are taken,
 * f(2) is the number of reachable positions if (2 * 131 + 65) steps are taken,
 * etc.
 * Considering that 26501365 = 202300 * 131 + 65, once finding a, b, and c, you just need
 * to compute f(202300).
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct pos_s {
    int row;
    int col;
} pos_t;

typedef enum pos_type_e {
    GARDEN_PLOT = '.',
    ROCK = '#'
} pos_type_t;

#define INPUT argv[1]
#define GRID_SIZE ((strlen(argv[1]) == 5)? 131 : 11)
#define QUEUE_SIZE 500000 // empirical
#define VALID_COORDS(row, col, len) (row >= 0 && row < len && col >= 0 && col < len)
#define ABS(x) (((x) < 0)? (-(x)) : (x))
#define DIST(p1, p2) (ABS((p1.row) - (p2.row))) + (ABS((p1.col) - (p2.col)))
#define STEPS_PART2 26501365

bool is_in_queue(pos_t* queue, int queue_size, pos_t p) {
    for (int i = 0; i < queue_size; i++) {
        if (queue[i].col == p.col && queue[i].row == p.row) {
            return true;
        }
    }
    return false;
}

void push(pos_t* queue, int* queue_size, pos_t p) {
    if (*queue_size >= QUEUE_SIZE) {
        perror("need to reallocate \n");
        exit(5);
    }
    queue[*queue_size].col = p.col;
    queue[*queue_size].row = p.row;
    *queue_size += 1;
}

pos_t recenter_oob_position(pos_t uncentered, int grid_dimension) {
    pos_t res = {
        .row = uncentered.row > 0 ? (uncentered.row % grid_dimension) : (uncentered.row % grid_dimension) + grid_dimension,
        .col = uncentered.col > 0 ? (uncentered.col % grid_dimension) : (uncentered.col % grid_dimension) + grid_dimension,
    };

    return res;
}

char get(char** grid, int grid_dimension, pos_t p) {
    if (VALID_COORDS(p.row, p.col, grid_dimension)) {
        return grid[p.row][p.col];
    } else {
        pos_t in_bounds = recenter_oob_position(p, grid_dimension);
        return grid[in_bounds.row][in_bounds.col];
    }
}

// Just a plain Breadth First Search where we keep track of the reachable positions,
// and after we reach the max steps, we count the number of positions for which the distance
// is below the number of required steps (while making sure the number of steps is even, didn't notice this at first)
long explore(char** grid, const int grid_dimension, pos_t start, int max_steps) {
    int neighbors[4][2] = {
        { -1, 0 }, // N
        { 0, +1 }, // E 
        { +1, 0 }, // S
        { 0, -1 }, // W
    };

    pos_t* seen = malloc(QUEUE_SIZE * sizeof(pos_t));
    memset(seen, 0, QUEUE_SIZE * sizeof(pos_t));
    int seen_size = 0;

    pos_t* reachable = malloc(QUEUE_SIZE * sizeof(pos_t));;
    memset(reachable, 0, sizeof(int) * grid_dimension * grid_dimension);
    int nb_reachable = 0;
    push(reachable, &nb_reachable, start);

    pos_t queue[QUEUE_SIZE] = { 0 };
    queue[0] = start;
    int queue_size = 1;

    int step = 0;
    long res = 1;
    while (step < max_steps) {
        pos_t next_queue[QUEUE_SIZE] = { 0 };
        int next_queue_size = 0;

        for (int i = 0; i < queue_size; i++) {
            pos_t current = queue[i];

            if (is_in_queue(seen, seen_size, current)) {
                continue;
            }

            for (int j = 0; j < 4; ++j) {
                pos_t neighbor = { .row = current.row + neighbors[j][0], .col = current.col + neighbors[j][1] };

#ifndef PART2
                if (!VALID_COORDS(neighbor.row, neighbor.col, grid_dimension)) {
                    continue;
                }
#endif

                if (get(grid, grid_dimension, neighbor) == ROCK) {
                    continue;
                }

                if (is_in_queue(seen, seen_size, neighbor)) {
                    continue;
                }

                next_queue[next_queue_size] = neighbor;
                next_queue_size++;

                if (!is_in_queue(reachable, nb_reachable, neighbor)) {
                    push(reachable, &nb_reachable, neighbor);
                    int d = DIST(neighbor, start);
                    if (d <= max_steps && d % 2 == 0) {
                        res++;
                    }
                }
            }

            push(seen, &seen_size, current);
        }

        memcpy(queue, next_queue, next_queue_size * sizeof(pos_t));
        queue_size = next_queue_size;

        step++;
    }

    free(seen);
    free(reachable);

    return res;
}

int main(int argc, char* argv[]) {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    char line[1000];
    int lines = 0;
    const int grid_dimension = GRID_SIZE;
    char* grid[grid_dimension];
    pos_t start = { 0 };

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        p[strcspn(p, "\n")] = 0;
        int len = strlen(p);

        grid[lines] = malloc(sizeof(char) * (len + 1));
        strncpy(grid[lines], p, len);
        grid[lines][len] = '\0';

        char* q;
        if ((q = strchr(p, 'S'))) {
            start.row = lines;
            start.col = q - p;
        }
        lines++;
    }

    printf("result part1: %ld\n", explore((char**) grid, grid_dimension, start, atoi(argv[2])));
#ifdef PART2
    long f0 = explore((char**) grid, grid_dimension, start, 65);
    long f1 = explore((char**) grid, grid_dimension, start, 65 + 131);
    long f2 = explore((char**) grid, grid_dimension, start, 65 + 131 * 2);

    long a = (f2 / 2) - f1 + f0 / 2;
    long b = f1 - f0 - a;
    long c = f0;

    // f(x) = ax^2 + bx + c
    long n = 202300; // = (STEPS_PART2 - 65) / 131
    long res = a * n * n + b * n + c;
    printf("result part2: %ld\n", res);
#endif
    fclose(f);
}
