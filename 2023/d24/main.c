#include <assert.h>
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

int LUPDecompose(long double **A, int N, int *P) {
    int i, j, k, imax;
    long double maxA, *ptr, absA;

    for (i = 0; i <= N; i++)
        P[i] = i; // Unit permutation matrix, P[N] initialized with N

    for (i = 0; i < N; i++) {
        maxA = 0.0;
        imax = i;

        for (k = i; k < N; k++)
            if ((absA = fabsl(A[k][i])) > maxA) {
                maxA = absA;
                imax = k;
            }

        if (imax != i) {
            // pivoting P
            j = P[i];
            P[i] = P[imax];
            P[imax] = j;

            // pivoting rows of A
            ptr = A[i];
            A[i] = A[imax];
            A[imax] = ptr;

            // counting pivots starting from N (for determinant)
            P[N]++;
        }

        for (j = i + 1; j < N; j++) {
            A[j][i] /= A[i][i];

            for (k = i + 1; k < N; k++)
                A[j][k] -= A[j][i] * A[i][k];
        }
    }

    return 1;  //decomposition done
}

void LUPSolve(long double **A, int *P, long double *b, int N, long double *x) {
    for (int i = 0; i < N; i++) {
        x[i] = b[P[i]];

        for (int k = 0; k < i; k++)
            x[i] -= A[i][k] * x[k];
    }

    for (int i = N - 1; i >= 0; i--) {
        for (int k = i + 1; k < N; k++)
            x[i] -= A[i][k] * x[k];

        x[i] /= A[i][i];
    }
}

void solve_AX_b(long double** a, long double* x, long double* b, const int n) {
    // Solve AX = b equation using LU decomposition
    // reuses LUPDecompose and LUPSolve functions
    // from https://en.wikipedia.org/wiki/LU_decomposition
    int* p = (int* ) malloc(n * sizeof(int));
    if (!p) {
        perror("Malloc P failed!");
    }
    LUPDecompose(a, 4, p);
    LUPSolve(a, p, b, 4, x);
    free(p);
}

void find_rock_to_throw(hailstone_t* rock) {
    // let h0 be the hailstone representing the rock we want to throw
    // let hi...hn be the hailstones from the input
    // we want to find (h0, v0) = (x0, y0, z0, v0x, v0y, v0z) such as
    // for each hailstone i, there exists a time tj such as (h0 collisions with hi):
    // (x0 + tj * v0x, y0 + tj * v0y, z0 + tj * v0z) = (xi + tj * vix, yi + tj * viy, zi + tj * viz)
    // i.e. (x0 - xi) = tj * (vix - v0x)
    //      (y0 - yi) = tj * (viy - v0y)
    //      (z0 - zi) = tj * (viz - v0z)
    // so (if v0y != viy)
    //  (x0 - xi) * (viy - v0y) = (vix - v0x) * (y0 - yi)
    //  viy * x0 - v0y * x0 - viy * xi + v0y * xi = vix * y0 - vix * yi - v0x * y0 + v0x * yi
    //  (viy - v0y)x0 + (v0y - viy)xi = (vix - v0x)y0 + (v0x - vix)yi
    //  rearrange to keep unknowns (x0, y0) at the left
    //  (viy - v0y)x0 + (v0x - vix)y0 = (viy - v0y)xi + (v0x - vix)yi
    //  viy*x0 - v0y*x0 + v0x*y0 - vix*y0 = viy*xi - v0y*xi + v0x*yi - vix*yi
    //  (multiply by -1)
    //  v0y*x0 - viy*x0 + vix*y0 - v0x*y0 = v0y*xi - viy*xi + vix*yi - v0x*yi
    //
    //
    //  if we go and consider (i, j) a couple of hailstones (forget about j being the current time stamp)
    //
    //  v0y*x0 - viy*x0 + vix*y0 - v0x*y0 = v0y*xi - viy*xi + vix*yi - v0x*yi
    //  v0y*x0 - vjy*x0 + vjx*y0 - v0x*y0 = v0y*xj - vjy*xj + vjx*yj - v0x*yj
    //
    //  subtract
    //  (viy - vjy)x0 + (vjx-vix)y0 = (xj - xi)v0y + viy*xi - vjy*xj + vjx*yj - vix*yi + (yi - yj)*v0x
    //  (viy - vjy)x0 + (vjx-vix)y0 + (yj - yi)*v0x + (xi - xj)v0y = viy*xi - vjy*xj + vjx*yj - vix*yi
    //           a*x0 +        b*y0 +         c*v0x +        d*v0y = C
    //
    //   this can be written as a matrix product equation
    //
    //                                              (x0 )   
    //                                              (y0 )
    //   (viy - vjy, vjx - vix, yj - yi, xi - xj) x (v0x) = (viy * xi - vjy * xj + vjx * yj - vix * yi)
    //                                              (v0y)
    //
    //   (c1, c2, c3, c4)  x X = (c5)
    //
    // Basically X has 4 variables so we need 4 equations to solve the equation.
    // So we need to loop over the hailstones pairwise and get the lhs and rhs (4 times)
    // sometimes this can't be achieved because viy == vjy or yi == yj (which is the condition listed above)
    // next we solve the equation, and apply the same method to determine z0, vz0

    // A
    long double** lhs_coeffs = (long double**) malloc(4 * sizeof(long double*));
    for (int _ = 0; _ < 4; _++) {
        lhs_coeffs[_] = (long double*) malloc(4 * sizeof(long double));
    }

    // B
    long double* rhs_coeffs = (long double* ) malloc(4 * sizeof(long double));

    // we need at least 4 couples of (hi, hj) hailstones
    // that have
    //      different vy and different y, to find (x0, y0, vx0, vy0)
    //      different vz and different z, to find (x0, z0, vz0, vz0)
    int nb_coeffs = 0;
    for (int i = 0; i < nb_hailstones && nb_coeffs < 4; i++) {
        hailstone_t* hi = &hailstones[i];
        for (int j = i + 1; j < nb_hailstones && nb_coeffs < 4; j++) {
            hailstone_t* hj = &hailstones[j];
            if (hi->vy == hj->vy || hi->y == hj->y)
                continue;

            lhs_coeffs[nb_coeffs][0] = (long double) hj->vy - (long double) hi->vy;
            lhs_coeffs[nb_coeffs][1] = (long double) hi->vx - (long double) hj->vx;
            lhs_coeffs[nb_coeffs][2] = (long double) hi->y - (long double) hj->y;
            lhs_coeffs[nb_coeffs][3] = (long double) hj->x - (long double) hi->x;

            rhs_coeffs[nb_coeffs] = (long double) (hi->y * hi->vx) - (long double) (hi->x * hi->vy) - (long double) ((hj->y * hj->vx) - (hj->x * hj->vy));
            nb_coeffs++;
        }
    }
    assert(nb_coeffs == 4);

    long double* x = malloc(4 * sizeof(long double));
    solve_AX_b(lhs_coeffs, x, rhs_coeffs, 4);
    rock->x = x[0]; // x0
    rock->y = x[1]; // y0
    rock->vx = x[2]; // vx0
    rock->vy = x[3]; // vy0

    // Likewise, get the coefficients necessary to get z (using x/z instead of x/y)
    nb_coeffs = 0;
    for (int i = 0; i < nb_hailstones && nb_coeffs < 4; i++) {
        hailstone_t* hi = &hailstones[i];
        for (int j = i + 1; j < nb_hailstones && nb_coeffs < 4; j++) {
            hailstone_t* hj = &hailstones[j];
            if (hi->vz == hj->vz || hi->z == hj->z)
                continue;

            lhs_coeffs[nb_coeffs][0] = (long double) hj->vz - (long double) hi->vz;
            lhs_coeffs[nb_coeffs][1] = (long double) hi->vx - (long double) hj->vx;
            lhs_coeffs[nb_coeffs][2] = (long double) hi->z - (long double) hj->z;
            lhs_coeffs[nb_coeffs][3] = (long double) hj->x - (long double) hi->x;

            rhs_coeffs[nb_coeffs] = (long double) (hi->z * hi->vx) - (long double) (hi->x * hi->vz) - (long double) ((hj->z * hj->vx) - (hj->x * hj->vz));
            nb_coeffs++;
        }
    }
    assert(nb_coeffs == 4);

    solve_AX_b(lhs_coeffs, x, rhs_coeffs, 4);
    rock->z = x[1];
    rock->vz = x[3];

    free(x);
    free(rhs_coeffs);
    for (int i = 0; i < 4; i++) {
        free(lhs_coeffs[i]);
    }
    free(lhs_coeffs);
}

int main() {
    FILE* f = fopen(INPUT, "r");

    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char* p = line;

        long long x, y, z;
        int vx, vy, vz;
        if (sscanf(p, "%lld, %lld, %lld @ %d, %d, %d", &x, &y, &z, &vx, &vy, &vz) == 6) {
            hailstone_t h = { .x = x, .y = y, .z = z, .vx = vx, .vy = vy, .vz = vz };
            hailstones = realloc(hailstones, (1 + nb_hailstones) * sizeof(hailstone_t));
            hailstones[nb_hailstones++] = h;
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


    hailstone_t rock;
    find_rock_to_throw(&rock);
    fprintf(stdout, "part2: %lld\n", (unsigned long long) rock.x + (unsigned long long) rock.y + (unsigned long long) rock.z);

    free(hailstones);
    fclose(f);
}
