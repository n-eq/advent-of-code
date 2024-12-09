#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#define NB_NODES 5
#define INPUT "input_test"
#elif defined(TEST2)
#define NB_NODES 10
#define INPUT "input_test2"
#elif defined(TEST3)
#define NB_NODES 20
#define INPUT "input_test3"
#else
#define NB_NODES 140
#define INPUT "input"
#endif

#define LOOP_LEN_MAX NB_NODES * NB_NODES
static char map[NB_NODES][NB_NODES];

typedef struct position_s {
    int line;
    int col;
} position_t;

#define BOUNDARY_CHECK(pi, pj) ((pi >= 0 && pi < NB_NODES && pj >= 0 && pj < NB_NODES))

static FILE* parse_input(position_t* start) {
    FILE *file = fopen(INPUT, "r");
    if (file == NULL) {
        perror("Error: can't open file.\n");
    }

    char line[256];

    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') {
            continue;
        }

        char* p = line;
        memcpy(map[i], p, NB_NODES);

        char* q;
        if ((q = strchr(line, 'S'))) {
            start->line = i;
            start->col = (int) (q - p);
        }

        i++;
    }
    return file;
}

// Neighbors are adjacent positions (N, W, E, S) that are not out of bounds
// and do not contain ground (.)
static int get_neighbors(position_t p, position_t* neighbors) {
    int count = 0;
    if (BOUNDARY_CHECK(p.line - 1, p.col)) {
        position_t new_pos = (position_t) { p.line - 1, p.col };
        if (map[new_pos.line][new_pos.col] != '.') {
            neighbors[count++] = new_pos;
        } else {

        }
    }
    if (BOUNDARY_CHECK(p.line + 1, p.col)) {
        position_t new_pos = (position_t) { p.line + 1, p.col };
        if (map[new_pos.line][new_pos.col] != '.') {
            neighbors[count++] = new_pos;
        }
    }
    if (BOUNDARY_CHECK(p.line, p.col - 1)) {
        position_t new_pos = (position_t) { p.line, p.col - 1 };
        if (map[new_pos.line][new_pos.col] != '.') {
            neighbors[count++] = new_pos;
        }
    }
    if (BOUNDARY_CHECK(p.line, p.col + 1)) {
        position_t new_pos = (position_t) { p.line, p.col + 1 };
        if (map[new_pos.line][new_pos.col] != '.') {
            neighbors[count++] = new_pos;
        }
    }
    return count;
}

// taken from https://wrfranklin.org/Research/Short_Notes/pnpoly.html
// basically a "finer" version of ray casting algorithm to determine if a point
// is inside a polygon
bool pnpoly(int nvert, int *vertx, int *verty, int testx, int testy) {
  int i, j;
  bool c = false;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
      if (((verty[i]>testy) != (verty[j]>testy)) &&
          (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) ) {
          c = !c;
      }
  }
  return c;
}

bool is_enclosed(int row, int col, position_t* polygon, int len) {
    // filter out points that are edges of the polygon
    for (int i = 0; i < len; i++) {
        if (polygon[i].line == row && polygon[i].col == col) {
            return false;
        }
    }

    int* loopx = malloc(sizeof(position_t) * len);
    int* loopy = malloc(sizeof(position_t) * len);

    for (int idx = 0; idx < len; idx++) {
        loopx[idx] = polygon[idx].line;
        loopy[idx] = polygon[idx].col;
    }

    return pnpoly(len, loopx, loopy, row, col);
}

static int position_sort(const void * a, const void * b) {
    position_t p1 = *(position_t* ) a;
    position_t p2 = *(position_t* ) b;

    if (p1.line != p2.line) {
        return p1.line - p2.line;
    } else {
        return p1.col - p2.col;
    }
}


int main() {
    position_t start;

    FILE* file = parse_input(&start);

    // We start from 'S' position, and for each neighbor, we compute the distance
    // to loop back again to the start position, we keep track of the maximum distance
    // based on each neighbor.
    position_t neighbors[4] = { 0 };
    int dist = 0;
    int count = get_neighbors(start, neighbors); // neighbors of starting position

    position_t prev = start;
    position_t loop[LOOP_LEN_MAX] = { 0 };
    int loop_len = 0;
    for (int i = 0; i < count; ++i) {
        int new_dist = 1;
        int pi = neighbors[i].line;
        int pj = neighbors[i].col;

        position_t tmp_loop[LOOP_LEN_MAX] = { 0 };
        int tmp_loop_len = 1;
        tmp_loop[0] = start;
        memset(tmp_loop + 1, 0, sizeof(position_t) * (NB_NODES - 1));
        while (1) {
            if (pi == start.line && pj == start.col) {
                break;
            }

            position_t positions[2] = { 0 };
            switch (map[pi][pj]) {
                case '-':
                    positions[0] = (position_t) { pi, pj - 1 };
                    positions[1] = (position_t) { pi, pj + 1 };
                    break;
                case '|':
                    positions[0] = (position_t) { pi - 1, pj };
                    positions[1] = (position_t) { pi + 1, pj };
                    break;
                case 'F':
                    positions[0] = (position_t) { pi + 1, pj };
                    positions[1] = (position_t) { pi, pj + 1 };
                    break;
                case 'J':
                    positions[0] = (position_t) { pi - 1, pj };
                    positions[1] = (position_t) { pi, pj - 1 };
                    break;
                case 'L':
                    positions[0] = (position_t) { pi - 1, pj };
                    positions[1] = (position_t) { pi, pj + 1 };
                    break;
                case '7':
                    positions[0] = (position_t) { pi + 1, pj };
                    positions[1] = (position_t) { pi, pj - 1 };
                    break;
                default:
                    break;
            }

            position_t tmp = (position_t) { pi, pj };
            if (BOUNDARY_CHECK(positions[0].line, positions[0].col) &&
                !(positions[0].line == prev.line && positions[0].col == prev.col) &&
                map[positions[0].line][positions[0].col] != '.') {
                pi = positions[0].line;
                pj = positions[0].col;
            } else if (BOUNDARY_CHECK(positions[1].line, positions[1].col) &&
                !(positions[1].line == prev.line && positions[1].col == prev.col) &&
                map[positions[1].line][positions[1].col] != '.') {
                pi = positions[1].line;
                pj = positions[1].col;
            } else {
                // this shouldn't be reachable
                break;
            }
            prev = tmp;

            tmp_loop[tmp_loop_len++] = (position_t) { tmp.line, tmp.col };
            new_dist++;
        }
        new_dist /= 2;

        if (new_dist > dist) {
            dist = new_dist;

            for (int i = 0; i < tmp_loop_len; ++i) {
                loop[i] = tmp_loop[i];
            }
            loop_len = tmp_loop_len;
        }
    }
    printf("part1: %d\n", dist);

    int enclosed = 0;
    for (int i = 0; i < NB_NODES; ++i) {
        for (int j = 0; j < NB_NODES; ++j) {
            if (is_enclosed(i, j, loop, loop_len)) {
                enclosed++;
            }
        }
    }
    printf("part2: %d\n", enclosed);

    fclose(file);
}
