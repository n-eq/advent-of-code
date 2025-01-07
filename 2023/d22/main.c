#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT argv[1]
#endif

typedef struct pos_s {
    int x;
    int y;
    int z;
} pos_t;

typedef struct brick_s {
    pos_t* pos1;
    pos_t* pos2;
} brick_t;

struct point_2d_t {
    double  x, y;
};

static brick_t* bricks;
static int* chain_reaction_res;
static int nb_bricks;

#define FMT_BRICK "%d,%d,%d~%d,%d,%d"
#define BRICK_POS_FMT(b) (b)->pos1->x, (b)->pos1->y, (b)->pos1->z, (b)->pos2->x, (b)->pos2->y, (b)->pos2->z

static void display_bricks() {
    for (int i = 0; i < nb_bricks; i++) {
        brick_t* b = &bricks[i];
        printf(FMT_BRICK "\n", BRICK_POS_FMT(b));
    }
    printf("______________\n");
}

// compare bricks based on their height (z value), used by qsort
static int cmp_bricks(const void* brick1, const void* brick2) {
    brick_t* b1 = (brick_t* ) brick1;
    brick_t* b2 = (brick_t* ) brick2;

    int z1[2] = { b1->pos1->z, b1->pos2->z };
    int z2[2] = { b2->pos1->z, b2->pos2->z };

    if (z1[1] < z1[0]) {
        return -1;
    } else if (z2[0] < z1[0]) {
        return 1;
    } else {
        return 0;
    }
}

int inside(int val, int intv1, int intv2) {
    return (intv1 <= val && val <= intv2);
}

// whether brick index would fall if every brick in hidden was ignored
int would_fall(int index, int* hidden, int hidden_count) {
    brick_t b;
    memcpy(&b, bricks + index, sizeof(brick_t));

    for (int i = 0; i < nb_bricks; i++) {
        int cont = 0;
        for (int j = 0; j < hidden_count; j++) {
            if (i == hidden[j]) {
                cont = 1;
                break;
            }
        }

        if (cont)
            continue;

        brick_t* bi = bricks + i;

        if (bi->pos1->z == b.pos1->z - 1 || bi->pos2->z == b.pos1->z - 1) {
            for (int x = bi->pos1->x; x <= bi->pos2->x; x++) {
                for (int y = bi->pos1->y; y <= bi->pos2->y; y++) {
                    // Brick b wouldn't fall, it's saved by brick bi
                    if (inside(x, b.pos1->x, b.pos2->x) && inside(y, b.pos1->y, b.pos2->y)) {
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

// Loop over the bricks in increasing z1 order,
// for each brick, move it down while it still can (floor reached or other brick blocking below)
void finish_falling() {
    for (int i = 0; i < nb_bricks; i++) {
        brick_t* bi = bricks + i;

        while (bi->pos1->z >= 2) {
            int below = bi->pos1->z - 1;
            int canmove = 1;

            for (int j = 0; j < nb_bricks; j++) {
                if (i == j)
                    continue;
                brick_t* bj = bricks + j;

                if (bj->pos2->z != below)
                    continue;

                for (int x = bi->pos1->x; x <= bi->pos2->x; x++) {
                    for (int y = bi->pos1->y; y <= bi->pos2->y; y++) {
                        if (inside(x, bj->pos1->x, bj->pos2->x) && inside(y, bj->pos1->y, bj->pos2->y)) {
                            canmove = 0;
                            break;
                        }
                    }
                    if (!canmove)
                        break;
                }
            }

            if (!canmove)
                break;

            bricks[i].pos1->z -= 1;
            bricks[i].pos2->z -= 1;
        }
        // the qsort is important, don't remove it!
        // however, it's only necessary to sort the lower part slice of the list [..i]
        qsort(bricks, i, sizeof(brick_t), cmp_bricks);
    }
}

int can_disintegrate(int index) {
    brick_t* b = bricks + index;
    pos_t start = *b->pos1;
    pos_t end = *b->pos2;

    // since bricks are sorted, only consider the bricks above
    for (int i = index + 1; i < nb_bricks; i++) {
        brick_t* bi = bricks + i;

        // yes they are sorted but some bricks can have the same z1 value,
        // we don't need to consider them
        if (bi->pos1->z == start.z)
            continue;

        pos_t p1 = *bi->pos1;
        pos_t p2 = *bi->pos2;

        if (would_fall(i, (int*) &index, 1)) {
            return 0;
        }
    }
    return 1;
}

int compare_int( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

int* chain_reaction(int index, int* blacklist, int* blacklist_size, int* count) {
    int* res = malloc(sizeof(int));
    int res_count = 0;

    brick_t* b = bricks + index;
    pos_t start = *b->pos1;
    pos_t end = *b->pos2;

    int* new_blacklist = malloc(*blacklist_size * sizeof(int));
    memcpy(new_blacklist, blacklist, *blacklist_size * sizeof(int));
    int new_size = *blacklist_size;
    for (int i = index + 1; i < nb_bricks; i++) {
        brick_t* bi = bricks + i;

        if (bi->pos1->z == start.z)
            continue;

        int skip = 0;
        for (int k = 0; k < res_count; k++) {
            if (res[k] == i)
                skip = 1;
        }

        if (skip)
            continue;

        if (would_fall(i, blacklist, *blacklist_size)) {
            new_blacklist = realloc(new_blacklist, (1 + new_size) * sizeof(int));
            new_blacklist[new_size++] = i;

            res = realloc(res, (1 + res_count) * sizeof(int));
            res[res_count] = i;
            res_count++;

            int falling_count = 0;
            int* falling = chain_reaction(i, new_blacklist, &new_size, &falling_count);
            int new_res = 0;

            for (int j = 0; j < falling_count; j++) {
                if (bsearch(falling + j, res, res_count, sizeof(int), compare_int))
                    continue;

                res = realloc(res, (1 + res_count) * sizeof(int));
                res[res_count++] = falling[j];
                new_res += 1;
            }
        }
    }

    *count = res_count;
    return res;
}

int main(int argc, char* argv[]) {
    FILE* f = fopen(INPUT, "r");

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        char* p = line;

        int x1, x2, y1, y2, z1, z2;
        sscanf(p, "%d,%d,%d~%d,%d,%d", &x1, &y1, &z1, &x2, &y2, &z2);

        bricks = realloc(bricks, (nb_bricks + 1) * sizeof(brick_t));

        brick_t* b = &bricks[nb_bricks];
        b->pos1 = malloc(sizeof(pos_t));
        b->pos2 = malloc(sizeof(pos_t));
        *(b->pos1) = (pos_t) { x1, y1, z1 };
        *(b->pos2) = (pos_t) { x2, y2, z2 };

        nb_bricks++;
    }

    qsort(bricks, nb_bricks, sizeof(brick_t), cmp_bricks);
    finish_falling();

    int part1 = 0;
    int part2 = 0;

    for (int i = nb_bricks - 1; i >= 0; i--) {
        if (can_disintegrate(i))
            part1++;

        int* blacklist = malloc(sizeof(int));
        blacklist[0] = i;
        int blacklist_size =1;
        int falling;

        int* bricks = chain_reaction(i, blacklist, &blacklist_size, &falling);
        part2 += falling;
    }
    printf("%d %d\n", part1, part2);

    for (int i = 0; i < nb_bricks; i++) {
        free(bricks[i].pos1);
        free(bricks[i].pos2);
    }
    free(bricks);
    fclose(f);
}
