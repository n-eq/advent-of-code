#include <limits.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

typedef struct pos_s {
    int x;
    int y;
} pos_t;

int main() {
    FILE* f = fopen(INPUT, "r");

    char line[128];
    int target_x[2] = {0};
    int target_y[2] = {0};
    fgets(line, sizeof(line), f);
    if (sscanf(line, "target area: x=%d..%d, y=%d..%d", target_x + 0, target_x + 1,
                target_y + 0, target_y + 1) != 4) {
        fprintf(stderr, "fprintf failed!");
        exit(1);
    }

    pos_t probe = { 0, 0 };

    // The problem can be solved using projectile motion laws
    // but it's a bit more complex than I expected. Instead both
    // parts can be solved in less than a second by brute-forcing.
    int part1 = INT_MIN;
    int part2 = 0;
    int vmax = 500;
    for (int vx = -vmax; vx <= vmax; vx++) {
        for (int vy = -vmax; vy <= vmax; vy++) {

            int v_x = vx;
            int v_y = vy;
            pos_t p = probe;
            int current_max = INT_MIN;

            int seen = 0;
            while (p.y >= target_y[0]) {
                // Keep track of the highest y value reached using vx, vy
                if (p.y > current_max) {
                    current_max = p.y;
                }

                // only update the currnt heighest y value if the initial velocities
                // can reach a point inside the target area
                if ((target_x[0] <= p.x && p.x <= target_x[1]) &&
                    (target_y[0] <= p.y && p.y <= target_y[1])) {
                    if (current_max >= part1) {
                        part1 = current_max;
                    }

                    if (!seen) {
                        part2++;
                        seen = 1;
                    }
                }

                p.x += v_x;
                p.y += v_y;

                if (v_x > 0) {
                    v_x -= 1;
                } else if (v_x < 0) {
                    v_x += 1;
                }
                
                v_y -= 1;
            }
        }
    }
    printf("%d %d\n", part1, part2);

    fclose(f);
}
