#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SUPPORTERS 2000

typedef struct pos_s {
    int x;
    int y;
    int z;
} pos_t;

typedef struct brick_s {
    pos_t* pos1;
    pos_t* pos2;
    int* supporting_bricks; // indexes of lower bricks that support this one
    int nb_supporting_bricks;
} brick_t;

#define FMT_BRICK "%d,%d,%d~%d,%d,%d"
#define BRICK_POS_FMT(b) b->pos1->x, b->pos1->y, b->pos1->z, b->pos2->x, b->pos2->y, b->pos2->z

static void display_bricks(brick_t* bricks, int nb_bricks) {
    for (int i = 0; i < nb_bricks; i++) {
        brick_t* b = &bricks[i];
        printf("%d,%d,%d~%d,%d,%d\n", b->pos1->x, b->pos1->y, b->pos1->z, b->pos2->x, b->pos2->y, b->pos2->z);
    }
    printf("______________\n");
}

// compare bricks based on their z value (used by qsort)
static int cmp_bricks(const void* brick1, const void* brick2) {
    brick_t* b1 = (brick_t* ) brick1;
    brick_t* b2 = (brick_t* ) brick2;

    int z1[2] = { b1->pos1->z, b1->pos2->z };
    int z2[2] = { b2->pos1->z, b2->pos2->z };

    if (z1[1] < z2[0]) {
        // brick 1 is strictly below brick 2
        return -1;
    } else if (z2[1] < z1[0]) {
        return 1;
    } else {
        return z1[0] - z2[0];
    }
}

double slope(int x1, int y1, int x2, int y2) {
    // vertical line, slope is infinity
    if (x2 == x1) {
        return NAN;
    }

    return ((double) (y2 - y1) / (double) (x2 - x1));
}

bool is_occupied(brick_t* bricks, int nb_bricks, pos_t p, int* occupier) {
    for (int i = 0; i < nb_bricks; i++) {
        brick_t* bi = &bricks[i];
        if ((p.x >= bi->pos1->x && p.x <= bi->pos2->x) &&
                (p.y >= bi->pos1->y && p.y <= bi->pos2->y) &&
                (p.z >= bi->pos1->z && p.z <= bi->pos2->z)) {
            *occupier = i;
            
            return true;
        }
    }
    return false;
}

bool can_go_down(brick_t* bricks, int nb_bricks, int i) {
    brick_t b = bricks[i];
    if (!(b.pos1->z - 1 >= 1)) {
        return false; // reached the floor
    }

    // if any position (x, y, z), where x ∈ [pos1->x, pos2x], y ∈ [pos1->y, pos2->y],
    // in the below level, is occupied by another brick then we can't move down
    for (int xi = b.pos1->x; xi <= b.pos2->x; xi++) {
        for (int yi = b.pos1->y; yi <= b.pos2->y; yi++) {
            for (int zi = b.pos1->z - 1; zi <= b.pos2->z -1; zi++) {
                int occupying_brick = -1;
                if (is_occupied(bricks, nb_bricks, (pos_t) { xi, yi, zi }, &occupying_brick)) {
                    if (occupying_brick != i) {
                        return false;
                    }
                }
            }
        }
    }

    b.pos1->z--;
    b.pos2->z--;
    return true;
}

void fall_down(brick_t* bricks, int nb_bricks, brick_t** new_bricks, int* new_nb) {
    int total = 0;
    for (int i = 0; i < nb_bricks; ++i) {
        brick_t* bi = &bricks[i];
        brick_t new_bi = bricks[i];

        while (can_go_down(bricks, nb_bricks, i));

        (*new_bricks)[total].pos1 = malloc(sizeof(pos_t));
        (*new_bricks)[total].pos1 = new_bi.pos1;
        (*new_bricks)[total].pos2 = malloc(sizeof(pos_t));
        (*new_bricks)[total].pos2 = new_bi.pos2;
        (*new_bricks)[total].supporting_bricks = malloc(MAX_SUPPORTERS);
        total++;
    }
    *new_nb = total;
}

void add_to_supporting_bricks(brick_t* b, int index) {
    for (int i = 0; i < b->nb_supporting_bricks; i++) {
        if (b->supporting_bricks[i] == index) {
            return;
        }
    }

    b->supporting_bricks[b->nb_supporting_bricks++] = index;
}

int count_disintegratables(brick_t* bricks, int nb) {
    int res = 0;

    // a non-disintegratable brick is one that, for any other brick, it's the only supporting brick
    for (int i = 0; i < nb; i++) {
        brick_t* bi = &bricks[i];
        bool is_non_disintegratable = false;
        for (int j = 0; j < nb; ++j) {
            if (i == j) {
                continue;
            }

            brick_t* bj = &bricks[j];
            if (bj->nb_supporting_bricks == 1) {
                if (bj->supporting_bricks[0] == i) {
                    // brick bi is the only brick supporting bj,
                    // therefore isn't not disintegratable
                    is_non_disintegratable = true;
                    break;
                }
            }
        }

        if (!is_non_disintegratable) {
            res++;
        }
    }
    return res;
}

void find_supporting_bricks(brick_t* bricks, int nb) {
    for (int i = 0; i < nb; i++) {
        brick_t* bi = &bricks[i];
        for (int j = 0; j < i; j++) {
            if (bricks[j].pos2->z != bi->pos1->z - 1) {
                continue;
            }

            int occupier = -1;
            for (int xi = bi->pos1->x; xi <= bi->pos2->x; xi++) {
                for (int yi = bi->pos1->y; yi <= bi->pos2->y; yi++) {
                    pos_t p = (pos_t) { xi, yi, bi->pos1->z -1 };
                    if (is_occupied(bricks, nb, p, &occupier)) {
                        // printf("%d is supporting %d\n", j, i);
                        add_to_supporting_bricks(bi, j);
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    char line[1000];
    int nb_bricks = 0;
    brick_t* bricks  = malloc(sizeof(brick_t) * 1500); // 1377 in the input
    while (fgets(line, sizeof(line), f)) {
        char* p = line;

        int x1, x2, y1, y2, z1, z2;
        sscanf(p, "%d,%d,%d~%d,%d,%d", &x1, &y1, &z1, &x2, &y2, &z2);

        brick_t* b = &bricks[nb_bricks];
        b->pos1 = malloc(sizeof(pos_t));
        b->pos2 = malloc(sizeof(pos_t));
        b->supporting_bricks = malloc(MAX_SUPPORTERS * sizeof(int));
        memset(b->supporting_bricks, -1, MAX_SUPPORTERS);
        b->nb_supporting_bricks = 0;
        *(b->pos1) = (pos_t) { x1, y1 , z1};
        *(b->pos2) = (pos_t) { x2, y2 , z2};
        nb_bricks++;
    }

    qsort(bricks, nb_bricks, sizeof(brick_t), cmp_bricks);
    // display_bricks(bricks, nb_bricks);

    brick_t* bricks_after_falldown = malloc(sizeof(brick_t) * 1500);
    int nb_bricks_final = 0;

    fall_down(bricks, nb_bricks, &bricks_after_falldown, &nb_bricks_final);
    // display_bricks(bricks_after_falldown, nb_bricks_final);

    find_supporting_bricks(bricks_after_falldown, nb_bricks_final);

    /*
    for (int i = 0; i < nb_bricks_final; i++) {
        brick_t* bi = &bricks_after_falldown[i];
        printf("brick %d is supported by (%d): ", i, bi->nb_supporting_bricks);
        for (int s = 0; s < bi->nb_supporting_bricks; s++) {
            printf(" %d ", bi->supporting_bricks[s]);
        }
        printf("\n");
    }
    */

    int res = count_disintegratables(bricks_after_falldown, nb_bricks_final);
    printf("part1 = %d\n", res); // still too high value

    for (int i = 0; i < nb_bricks; i++) {
        free(bricks[i].pos1);
        free(bricks[i].pos2);
    }
    // TODO for other bricks too
    free(bricks);
    free(bricks_after_falldown);
    fclose(f);
}
