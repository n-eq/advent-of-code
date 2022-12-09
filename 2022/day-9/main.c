// compile & run: gcc main.c && ./a.out
#include <math.h>

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


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

void update_tail_coords(coords_t head, coords_t* tail) 
{
    // printf("before update %d %d %d %d!\n", head_j, head_i, *tail_j, *tail_i);

    float distance = fabs(sqrt(pow(head.j - tail->j, 2) + pow(head.i - tail->i, 2)));
    if (distance < 2.0f) {
        // printf("they're touching, returning;\n");
        return;
    }

    tail->i += 1 * sign(head.i - tail->i);
    tail->j += 1 * sign(head.j - tail->j);

    // printf("after update %d %d %d %d!\n", head_j, head_i, *tail_j, *tail_i);
}

void update_tails_coords(coords_t** tails) {
    coords_t* tail = *tails;

    while (tail) {
        update_tail_coords(*tail, tail + 1);
        tail++;
    }
}


#if 0
char visited[MAX_GRID][MAX_GRID] = {0};
void display_grid(int head_row, int head_col, int tail_row, int tail_col, bool override) {
    for (int i = 0; i < MAX_GRID; ++i) {
        int add = 0;
        for (int j = 0; j < MAX_GRID; ++j) {
            char c = '.';
            if ((i == MAX_GRID - 1) && (j == 0)) {
                c = 's';
            } else {
                if (i == head_row && j == head_col) {
                    c = 'H';
                }

                if (i == tail_row && j == tail_col) {
                    if (c == '.') {
                        c = 'T';
                    } else {
                        add = 1;
                        // collision
                    }
                }

            }

            if (override && visited[i][j] > 0) {
                c = '#';
            }

            printf("%c", c);
        }
        if (add == 1) {
            printf("\t Collision");
        }
        printf("\n");
    }
    printf("\n");
}
#endif

int main() {
    char buf[8];

    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("Opening input file");
        exit(1);
    }

    char grid[MAX_GRID][MAX_GRID] = {0};

    long sum = 0;

    coords_t head = { .j = MAX_GRID - 1, .j = 0 };
    coords_t tail = { .j = MAX_GRID - 1, .j = 0 };

    coords_t head_and_tails[10] = { 0 };


    coords_t visited[MAX_GRID * MAX_GRID];
    int total = 0;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        // strip trailing newline character
        buf[strcspn(buf, "\n")] = 0;

        if (buf[0] == '\0') {
            continue;
        }

        // printf("buf: %s\n", buf);
        char movement = 0;
        int steps = 0;

        sscanf(buf, "%c %d", &movement, &steps);

        printf("~~~~%d %c\n", steps, movement);

        for (int i = 0; i < steps; ++i) {
            printf("coords before: head (%c): (%d,%d), tail: (%d,%d)\n", movement, head.j, head.i, tail.j, tail.i);
            switch (movement) {
                case 'R':
                    head.j++;
                    break;
                case 'L':
                    head.j--;
                    break;
                case 'U':
                    head.i--;
                    break;
                case 'D':
                    head.i++;
                    break;
            }

            update_tail_coords(head, &tail);

            update_tails_coords((coords_t** ) &head_and_tails);

            bool found  = false;
            for (int j = 0; j < total; ++j) {
                if (visited[j].i == tail.i && visited[j].j == tail.j)
                    found = true;
            }

            if (!found) {
                printf("new visited: %d %d\n", total - 1, i);
                visited[total - 1].i = tail.i;
                visited[total - 1].j = tail.j;
                total++;
            }
        }
        printf("======\n\n");
    }
    printf("total: %d\n", total);
    
    fclose(fp);
}

