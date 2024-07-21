#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#define NB_GALAXIES 9
#else
#define INPUT "input"
#define NB_GALAXIES 440 // sometimes it's ok to make assumptions based on the input and motivated by laziness
#endif

#ifndef EXPANSION_FACTOR
#ifdef PART2
#define EXPANSION_FACTOR 1000000
#else
#define EXPANSION_FACTOR 2
#endif
#endif

typedef struct galaxy_s {
    int row;
    int col;
} galaxy_t;

bool needs_to_be_expanded(int idx, int* list, int len) {
    for (int i = 0; i < len; ++i) {
        if (idx == list[i]) {
            return true;
        }
    }
    return false;
}

static char* universe[1000] = {0};
static galaxy_t* galaxies;

int main() {
    FILE *f = fopen(INPUT, "r");
    if (f == NULL) {
        return 1;
    }

    int nb_lines = 0;

    int expand_row_count = 0;
    int rows_to_expand[1000] = {0};
    
    int rows = 0;
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\n') {
            continue;
        }

        universe[nb_lines] = malloc(strlen(line) + 1);
        strcpy(universe[nb_lines++], line);

        char* p = line;
        char* q = strchr(p, '#');
        if (q == NULL) {
            rows_to_expand[expand_row_count++] = rows;
        }
        rows++;
    }


    // Detecting empty columns can only be done after the universe has been fully read
    int expand_col_count = 0;
    int is_in_list[1000] = {0};
    size_t universe_col_len = strlen(universe[0]);
    for (int col = 0; col < universe_col_len; ++col) {
        bool blank_col = true;
        for (int j = 0; j < nb_lines; ++j) {
            if (universe[j][col] != '.') {
                blank_col = false;
                break;
            }
        }

        if (blank_col) {
            is_in_list[expand_col_count++] = col;
        }
    }
    
    // Determine all the galaxies and their coordinates
    galaxies  = malloc(sizeof(galaxy_t) * NB_GALAXIES);
    int nb_galaxies = 0;
    for (int i = 0; i < nb_lines; ++i) {
        char* p = universe[i];
        int j = 0;
        while (p[j]) {
            if (p[j] == '#') {
                galaxies[nb_galaxies++] = (galaxy_t) { i, j };
            }
            ++j;
        }
    }

    long sum_dist_pairs = 0;
    for (int i = 0; i < nb_galaxies; ++i) {
        for (int j = 0; j < nb_galaxies; ++j) {
            if (i == j) {
                continue;
            }
            int min_row = galaxies[i].row < galaxies[j].row ? galaxies[i].row : galaxies[j].row;
            int max_row = galaxies[i].row > galaxies[j].row ? galaxies[i].row : galaxies[j].row;
            int min_col = galaxies[i].col < galaxies[j].col ? galaxies[i].col : galaxies[j].col;
            int max_col = galaxies[i].col > galaxies[j].col ? galaxies[i].col : galaxies[j].col;

            // The algorith is simple, we just need to calculate the distance between each pair of galaxies
            // Previously, we stored the indices of the rows/columns that need to be expanded.
            // The distance (shortest path) between two galaxies is the path measured along axes at right angles.
            // If the path crosses a row/column that needs to be expanded, we add the expansion factor to the distance.
            // For part1, EXPANSION_FACTOR is 2, for part2, it's 1000000
            long dist = 0;
            for (int in_between_row = min_row; in_between_row < max_row; ++in_between_row) {
                if (needs_to_be_expanded(in_between_row, rows_to_expand, expand_row_count)) {
                    dist += EXPANSION_FACTOR;
                } else {
                    dist++;
                }
            }

            for (int in_between_col = min_col; in_between_col < max_col; ++in_between_col) {
                if (needs_to_be_expanded(in_between_col, is_in_list, expand_col_count)) {
                    dist += EXPANSION_FACTOR;
                } else {
                    dist++;
                }
            }
            sum_dist_pairs += dist;
        }
    }

    printf("sum dist of shortest paths: %ld\n", sum_dist_pairs / 2);

    for (int i = 0; i < nb_lines; ++i) {
        free(universe[i]);
    }
    free(galaxies);

    fclose(f);
    return 0;
}
