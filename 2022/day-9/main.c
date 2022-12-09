// compile & run: gcc main.c -lm && ./a.out
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TAIL_LENGTH 9 /* part2: 9 */

#if 1
#define INPUT_FILE "input"
#define MAX_GRID 100
#else
#define INPUT_FILE "input_test"
#define MAX_GRID 6
#endif

// 0 if equal
int sign(int a) {
    return (a > 0) - (a < 0);
}

typedef struct coords {
    int i;
    int j;
} coords_t;

void update_tail_coords(coords_t head, coords_t* tail) {
    float distance = fabs(sqrt(pow(head.j - tail->j, 2) + pow(head.i - tail->i, 2)));
    if (distance < 2.0f) {
        return;
    }

    tail->i += 1 * sign(head.i - tail->i);
    tail->j += 1 * sign(head.j - tail->j);
}

int main() {
    char buf[8];

    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("Opening input file");
        exit(1);
    }

    long sum = 0;

    /* Part1: size = 2 (head & tail), part2: size = 10 (head & 9 tails) */
    coords_t head_and_tails[TAIL_LENGTH + 1] = { 0 };

    /* init head and first tail at the starting position first col bottom line */
    head_and_tails[0].i = MAX_GRID - 1;
    head_and_tails[0].j = 0;
    head_and_tails[1].i = MAX_GRID - 1;
    head_and_tails[1].j = 0;

    coords_t visited[MAX_GRID * MAX_GRID];
    int total = 0;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        // strip trailing newline character
        buf[strcspn(buf, "\n")] = 0;

        if (buf[0] == '\0') {
            continue;
        }

        char movement = 0;
        int steps = 0;
        sscanf(buf, "%c %d", &movement, &steps);

        for (int i = 0; i < steps; ++i) {
            switch (movement) {
                case 'R':
                    head_and_tails[0].j++;
                    break;
                case 'L':
                    head_and_tails[0].j--;
                    break;
                case 'U':
                    head_and_tails[0].i--;
                    break;
                case 'D':
                    head_and_tails[0].i++;
                    break;
            }

            for (int j = 0; j < TAIL_LENGTH; ++j) {
                update_tail_coords(head_and_tails[j], &head_and_tails[j + 1]);
            }

            bool found  = false;
            coords_t last_tail = head_and_tails[TAIL_LENGTH];
            for (int j = 0; j < total; ++j) {
                if (visited[j].i == last_tail.i && visited[j].j == last_tail.j)
                    found = true;
            }

            if (!found) {
                memcpy(&visited[total++ - 1], &last_tail, sizeof(coords_t));
            }
        }
    }
    printf("result: %d\n", total);
    
    fclose(fp);
}

