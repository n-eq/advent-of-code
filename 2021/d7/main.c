#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_input(char* input, int* positions, int* count) {
    char *p;
    p = strtok (input, ",");
    while (p) {
        positions[(*count)++] = atoi(p);
        p = strtok (NULL, ",");
    }
}

static int cmp(const void* p1, const void* p2) {
    const int* a = (const int*) p1;
    const int* b = (const int*) p2;
    return *a - *b;
}

void min_fuel(int* positions, int count, unsigned int* part1, unsigned int* part2) {
#include <limits.h>
    unsigned int min1 = UINT_MAX;
    unsigned int min2 = UINT_MAX;

    qsort(positions, count, sizeof(int), cmp);
    unsigned int range_start = positions[0];
    unsigned int range_end = positions[count - 1];

    for (int r = range_start; r < range_end; r++) {

        unsigned int tmp = 0;
        unsigned int tmp2 = 0;
        for (int i = 0; i < count; i++) {
            if (r <= positions[i]) {
                tmp += positions[i] - r;
                tmp2 += (positions[i] - r + 1) * (positions[i] - r) / 2;
            } else {
                tmp += r - positions[i];
                tmp2 += (r - positions[i] + 1) * (r - positions[i]) / 2;
            }
        }

        if (tmp < min1) {
            min1 = tmp;
        }
        if (tmp2 < min2) {
            min2 = tmp2;
        }
    }

    *part1 = min1;
    *part2 = min2;
}

int main() {
    /* FILE *file = fopen("input_test", "r"); */
    FILE *file = fopen("input", "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return 1;
    }

    long length = 0;
    fseek(file, 0, SEEK_END);
    length = ftell (file);
    fseek(file, 0, SEEK_SET);
    char* line = (char* ) malloc(length * sizeof(char));
    if (!fread(line, 1, length, file)) {
        printf("Failed to read input file");
        exit(1);
    }

    int* positions = (int* ) malloc(1000 * sizeof(int));
    int pos_count = 0;
    parse_input(line,positions, &pos_count);

    unsigned int part1 = 0;
    unsigned int part2 = 0;
    min_fuel(positions, pos_count, &part1, &part2);
    printf("part1: %d, part2: %d\n", part1, part2);

    free(line);
    free(positions);
    fclose(file);
    return 0;
}
