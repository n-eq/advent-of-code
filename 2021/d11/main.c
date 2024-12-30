#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define IN_BOUNDS(i, j) ((i) >= 0 && (i) < 10 && (j) >= 0 && (j) < 10)

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

int** map = NULL;

typedef struct pos_s {
    int row;
    int col;
} pos_t;

static void display_map() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            // '*' -> waiting to flash
            printf("%c", (map[i][j] < 10)? map[i][j] + '0' : '*');
        }
        printf("\n");
    }
    printf("\n");
}

static void flash(int row, int col, int** flashed) {
    // -1, -1  (-1, 0) (-1, 1 )  (0, -1), (0, 1) (1, -1), (1, 0), (1, 1)
    int nrows[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
    int ncols[] = { -1,  0,  1, -1, 1, -1, 0, 1 };

    // queue of elements waiting to flash during the current step
    pos_t* to_flash = malloc(10 * 10 * sizeof(pos_t));
    to_flash[0].row = row;
    to_flash[0].col = col;
    int nb_to_flash = 1;

    while (nb_to_flash) {
        pos_t current = to_flash[0];
        if (--nb_to_flash > 0) {
            to_flash = memmove(to_flash, to_flash + 1, nb_to_flash * sizeof(pos_t));
        }

        for (int n = 0; n < 8; n++) {
            int ni = current.row + nrows[n];
            int nj = current.col + ncols[n];
            
            if (!IN_BOUNDS(ni, nj)) {
                continue;
            }

            if (flashed[ni][nj]) {
                continue;
            }

            map[ni][nj] += 1;

            // insert the new element to the queue of elements to flash
            // if and only if its current energy level is greater than 9,
            //                it has not yet flashed in the current step
            //                it's not already enqueued
            if (map[ni][nj] > 9 && !flashed[ni][nj]) {
                int already_in_the_queue = 0;
                for (int i = 0; i < nb_to_flash; i++) {
                    if (to_flash[i].row == ni && to_flash[i].col == nj) {
                        already_in_the_queue = 1;
                        break;
                    }
                }

                if (!already_in_the_queue) {
                    to_flash[nb_to_flash] = (pos_t) { .row = ni, .col = nj };
                    nb_to_flash++;
                }
            }
        }
        flashed[current.row][current.col] = 1;
        map[current.row][current.col] = 0;
    }

    free(to_flash);
}

static int simulate(int* part1, int* part2) {
    int** flashed = calloc(10, sizeof(int* ));
    for (int i = 0; i < 10; i++) {
        flashed[i] = calloc(10, sizeof(int));
    }

    int flash_count = 0;
    int step = 1;
    while (1) {
        for (int i = 0; i < 10; i++) {
            memset(flashed[i], 0, 10 * sizeof(int));
        }

        /* printf("STEP %d\n", step); */
        /* display_map(); */

        // increase energy levels of all elements
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                map[i][j] += 1;
            }
        }

        // call flash on elements that have a high energy level
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (map[i][j] > 9) {
                    flash(i, j, flashed);
                }
            }
        }


        int sync = 1;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (flashed[i][j]) {
                    flash_count++;
                }

                // if any enegy level is not 0 after flashing,
                // the map isn't synced yet
                if (map[i][j] != 0) {
                    sync = 0;
                }
            }
        }

        if (step == 100) {
            *part1 = flash_count;
        }

        if (sync) {
            *part2 = step;
            break;
        }

        step++;
    }
    return flash_count;
}

int main() {
    FILE* f = fopen(INPUT, "r");

    map = (int**) malloc(10 * sizeof(int*));

    int lines = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';

        map[lines] = (int* ) malloc(10 * sizeof(int));

        for (size_t i = 0; i < strlen(line); i++) {
            map[lines][i] = line[i] - '0';
        }

        lines++;
    }

    int part1, part2;
    simulate(&part1, &part2);
    printf("%d %d\n", part1, part2);

    if (map) {
        for (int i = 0; i < 10; i++) {
            if (map[i])
                free(map[i]);
        }
        free(map);
    }
    fclose(f);
}
