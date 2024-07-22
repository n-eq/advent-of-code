#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef TEST
#define INPUT "input_test"
#define FIELD_SIZE 100
#else
#define INPUT "input"
#define FIELD_SIZE 100000
#endif

#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef enum direction_e {
    RIGHT = 'R',
    LEFT = 'L',
    UP = 'U',
    DOWN = 'D',
} direction_t;

typedef struct instruction_s {
    direction_t dir;
    long long meters;
} instruction_t;

typedef struct position_s {
    long long row;
    long long col;
} position_t;

position_t* process_inst(instruction_t inst, position_t curr) {
    position_t* ret = malloc(inst.meters * sizeof(position_t));
    for (int i = 0; i < inst.meters; ++i) {
        ret[i] = curr;
    }
    switch (inst.dir) {
    case RIGHT:
        for (int i = 0; i < inst.meters; ++i) {
            ret[i].col += i + 1;
        }
        break;
    case LEFT:
        for (int i = 0; i < inst.meters; ++i) {
            ret[i].col -= i + 1;
        }
        break;
    case UP:
        for (int i = 0; i < inst.meters; ++i) {
            ret[i].row -= i + 1;
        }
        break;
    case DOWN:
        for (int i = 0; i < inst.meters; ++i) {
            ret[i].row += i + 1;
        }
        break;
    }
    return ret;
}

long long dot(position_t p1, position_t p2) {
    return p1.row * p2.col - p1.col * p2.row;
}

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    char line[1000];

    position_t curr = { 0, 0 };
    long long res = 0;
    long long nb_points = 0;
    while (fgets(line, sizeof(line), f)) {
        char* p = line;

        instruction_t inst;

#ifndef PART2
        inst.dir = *p;
        inst.meters = atoi(p + 2);
#else
        //  0 means R, 1 means D, 2 means L, and 3 means U.
        char* q = strchr(p, ')') - 1;
        if (*q == '0') {
            inst.dir = 'R';
        } else if (*q == '1') {
            inst.dir = 'D';
        } else if (*q == '2') {
            inst.dir = 'L';
        } else if (*q == '3') {
            inst.dir = 'U';
        } else {
            perror("invalid direction");
            exit(5);
        }

        q = strchr(p, '(') + 7;
        char* par_start = strchr(p, '(');
        *q = '\0'; 

        inst.meters = strtol(par_start + 2, NULL, 16);
        // printf("%c %lld, p = %s\n", inst.dir, inst.meters, p);
#endif
        long long dig_length = inst.meters;

        position_t* next = process_inst(inst, curr);
        nb_points += dig_length;
        for (int i = 0; i < dig_length; ++i) {
            res += dot(next[i], next[(i + 1) % (dig_length)]);
        }
        res += dot(next[dig_length - 1], curr); // loop against the first element

        curr.row = next[dig_length - 1].row;
        curr.col = next[dig_length - 1].col;

        free(next);
    }
    // until here, res is the sum of the inside of the dug area

    // next, we need to add the perimerter of the dug area (number of points)
    res = (ABS(res) + nb_points) / 2 + 1;

#ifndef PART2
    printf("Part1: %lld\n", res);
#else
    printf("Part2: %lld\n", res);
#endif

    fclose(f);
}
