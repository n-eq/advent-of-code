#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAP_SIZE 100

#define IN_BOUNDS(i, j) ((i) >= 0 && (i) < MAP_SIZE && (j) >= 0 && (j) < MAP_SIZE)
#define NEIGHBORS(i, j) { { i - 1, j }, { i + 1, j } , { i, j - 1 }, { i, j +1 } }

int cmp_basin(const void* b1, const void* b2) {
    int size1 = (int) *(int* ) b1;
    int size2 = (int) *(int* ) b2;
    return (size1 - size2);
}

int get_basin_size(int** map, int row, int col, int** used) {
    int size = 1;
    used[row][col] = 1;
    int neighbors[4][2] = NEIGHBORS(row, col);
    for (int n = 0; n < 4; n++) {
        int ni = neighbors[n][0];
        int nj = neighbors[n][1];

        if (!IN_BOUNDS(ni, nj)) {
            continue;
        }

        if (used[ni][nj]) {
            continue;
        }

        if (map[ni][nj] != 9) {
            size += get_basin_size(map, ni, nj, used);
        }
    }

	return size;
}

int main() {
    FILE *file = fopen("input", "r");

    char* line = NULL;
    int** map = (int** ) malloc(MAP_SIZE * sizeof(int*));
    for (int i = 0; i < MAP_SIZE; i++) {
        map[i] = (int* ) malloc(MAP_SIZE * sizeof(int));
    }

    unsigned int part1 = 0;
    unsigned int part2 = 0;

    int i = 0;
    size_t len = 0;
    while (getline(&line, &len, file) != -1) {
        // strip trailing newline from the nend
        line[strcspn(line, "\n")] = 0;

        for (size_t c = 0; c < strlen(line); c++) {
            map[i][c] = line[c] - '0';
        }
        i++;
    }

    int** used = (int** ) malloc(MAP_SIZE * sizeof(int*));
    for (int i = 0; i < MAP_SIZE; i++) {
        used[i] = (int* ) malloc(MAP_SIZE * sizeof(int));
        memset(used[i], 0, MAP_SIZE * sizeof(int));
    }

    int* basin_sizes = (int* ) malloc(MAP_SIZE * MAP_SIZE * sizeof(int));
    int nb_basins = 0;
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            int height = map[i][j];
            int neighbors[4][2] = NEIGHBORS(i, j);
            int low = 1;
            for (int n = 0; n < 4; n++) {
                int ni = neighbors[n][0];
                int nj = neighbors[n][1];

                if (!IN_BOUNDS(ni, nj)) {
                    continue;
                }

                if (map[ni][nj] <= height) {
                    low = 0;
                    break;
                }
            }

            if (low) {
                part1 += height + 1;
                basin_sizes[nb_basins++] = get_basin_size(map, i, j, used);
            }
        }
    }

    qsort((void* ) basin_sizes, nb_basins, sizeof(int), cmp_basin);
    part2 = basin_sizes[nb_basins - 1] * basin_sizes[nb_basins - 2] * basin_sizes[nb_basins - 3];

    printf("%d %d\n", part1, part2);


    if (line) {
        free(line);
    }

    for (int i = 0; i < MAP_SIZE; i++) {
        free(used[i]);
        free(map[i]);
    }
    free(basin_sizes);
    free(map);
    free(used);

    fclose(file);
    return 0;
}
