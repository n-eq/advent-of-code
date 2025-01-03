#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#define TEST_MIN_AREA  7
#define TEST_MAX_AREA  27
#else
#define INPUT "input"
#define TEST_MIN_AREA 200000000000000
#define TEST_MAX_AREA 400000000000000
#endif

typedef struct hailstone_s {
    long long x; long long y; long long z;
    int vx; int vy; int vz;
} hailstone_t;

struct point_2d_t {
    double x, y;
};

hailstone_t* hailstones = NULL;
int nb_hailstones = 0;

double slope(hailstone_t* h) {
    // slope between (x, y) and (x + vx, y + vy)
    double p1[] = { (double) h->x, (double) h->y };
    double p2[] = { (double) h->x + (double) h->vx, (double) h->y + (double) h->vy };

    if (p1[0] - p2[0] == 0) {
        fprintf(stderr, "Slope of %lld, %lld is NAN!\n", h->x, h->y);
        return NAN;
    } else {
        return (p1[1] - p2[1]) / (p1[0] - p2[0]);
    }
}

struct point_2d_t intersection_2d(double a1, double b1, double a2, double b2) {
    // find the intersection (assume it exists) point of the lines
    // with the following linear equations
    // y = a1x + b1
    // y = a2x + b2
    struct point_2d_t intersection;

    // unique solution to the equation a1x + b1 = a2x + b2
    intersection.x = (b2 - b1) / (a1 - a2);
    intersection.y = a1 * ((b2 - b1) / (a1 - a2)) + b1;

    return intersection;
}


int intersect_in_test_area(hailstone_t* h1, hailstone_t* h2) {
    double slope1 = slope(h1);
    double slope2 = slope(h2);

    if (slope1 == slope2) {
        // Hailstones are parallel, they never intersect
        return 0;
    }

    struct point_2d_t intersection;

    // write y = a1x + b1 and y = a2x + b2 linear equations for both hailstones based on their initial values and velocities
    double a1 = slope1;
    double a2 = slope2;
    double b1 = (h1->y - slope1 * h1->x);
    double b2 = (h2->y - slope2 * h2->x);

    // unique solution to the equation a1x + b1 = a2x + b2
    intersection = intersection_2d(a1, b1, a2, b2);

    fprintf(stderr, "intersection (%lld, %lld, %d %d)/(%lld, %lld, %d %d): (%f, %f)\n",
            h1->x, h1->y, h1->vx, h1->vy, h2->x, h2->y, h2->vx, h2->vy, intersection.x, intersection.y);

    if ((intersection.x < TEST_MIN_AREA) || (intersection.y < TEST_MIN_AREA)
        || (intersection.x > TEST_MAX_AREA) || (intersection.y > TEST_MAX_AREA)
        // intersection point should be "in the future", it should be greater than
        // each of the hailstones x initial value if their respective velocity is positive, lower otherwise
        || (h1->vx * (intersection.x - h1->x) < 0) || (h2->vx * (intersection.x - h2->x) < 0)) {
        return 0;
    } else {
        return 1;
    }
}

int main() {
    FILE* f = fopen(INPUT, "r");

    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char* p = line;

        long long x, y, z;
        int vx, vy, vz;
        int res = sscanf(p, "%lld, %lld, %lld @ %d, %d, %d", &x, &y, &z, &vx, &vy, &vz);
        if (res == 6) {
            hailstone_t h = { .x = x, .y = y, .z = z, .vx = vx, .vy = vy, .vz = vz };
            hailstones = realloc(hailstones, (1 + nb_hailstones) * sizeof(hailstone_t));
            hailstones[nb_hailstones++] = h;
        } else {
            fprintf(stderr, "Failed to parse %s, got: %d\n", p, res);
            exit(EXIT_FAILURE);
        }

    }

    int part1 = 0;
    for (int i = 0; i < nb_hailstones - 1; i++) {
        for (int j = i + 1; j < nb_hailstones; j++) {
            if (intersect_in_test_area(hailstones + i, hailstones + j))
                part1++;
        }
    }
    fprintf(stdout, "part1: %d\n", part1);

    free(hailstones);
    fclose(f);
}
