// Well, this was the first time I hit a wall in 2023's exercises.
// Spent many days on this, rewrote it a few times, even tried Rust for the sake of it but
// I couldn't wrap my head around it.
// I had to look for help on Reddit. Eventually, a redditor with a great deal of patience (thanks Boojum!)
// provided me with a minimal reproducible example where I knew the expected result and from there I figured it out. It turned out
// that `seen` queue wasn't typed correctly, instead of only saving posdir_t
// (position, direction coming from, consecutive times we've used this direction),
// we also needed to save the distance to the source. This was the key to the solution, as I
// underexploring some paths because I was considering the same position with different distances.
// In terms of optimization and efficiency, the code has plenty of room for optimization, it currently takes around
// 6 mins to execute both parts. I'm sure it can be done in a fraction of that time.
// For example, (especially for part2), determining neighbors can be simplified by only considering
// the neighbors that satisfy (must at least be moving min_consecutive times in the same direction, except for the
// end position), which would highly reduce the number of explored positions/directions/consecutive times.
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#ifdef TEST
#define INPUT "input_test"
#define GRID_SIZE 13
#else
#define GRID_SIZE 141
#define INPUT "input"
#endif
#define QUEUE_SIZE 500 * GRID_SIZE * GRID_SIZE
#define NB_CARDINAL_DIRECTIONS 4

#define NL printf("\n")
#define VALID_COORDS(row, col) (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE)

typedef struct pos_s {
    int row;
    int col;
} pos_t;

typedef enum direction_e {
    NORTH = '^',
    EAST = '>',
    SOUTH = 'v',
    WEST = '<',
    NONE = ' ',
} direction_t;

// auxiliary structure to store a position and direction
// for any given position on the path
typedef struct posdir_s {
    pos_t pos;
    direction_t dir;
    int consecutive; // number of consecutive times we've used this direction
} posdir_t;

int grid[GRID_SIZE][GRID_SIZE] = {0};

char idx_to_dir(int idx) {
    switch (idx) {
    case 0:
        return '^';
    case 1:
        return '>';
    case 2:
        return 'v';
    case 3:
        return '<';
    default:
        return '^';
    }
}

int dir_to_idx(char c) {
    switch (c) {
    case '^':
        return 0;
    case '>':
        return 1;
    case 'v':
        return 2;
    case '<':
        return 3;
    default:
        return 0;
    }
}

pos_t convert_dir(char c) {
    switch (c) {
    case '>':
        return (pos_t) {0, 1};
    case '<':
        return (pos_t) {0, -1};
    case 'v':
        return (pos_t) {1, 0};
    case '^':
        return (pos_t) {-1, 0};
    default:
        return (pos_t) {0, 0};
    }
}

void display_grid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%d", grid[i][j]);
        }
        NL;
    }
}

posdir_t pop(posdir_t* queue, int* queue_size) {
    posdir_t res = queue[0];
    for (int i = 0; i < *queue_size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    *queue_size -= 1;
    return res;
}

void push(posdir_t* queue, posdir_t posdir, int* queue_size) {
    if (*queue_size >= QUEUE_SIZE) {
        printf("‼️  queue is full\n");
        exit(1);
    }
    queue[*queue_size] = posdir;
    *queue_size += 1;
}

bool is_enqueued(posdir_t* queue, posdir_t posdir, int queue_size) {
    for (int i = 0; i < queue_size; i++) {
        if (queue[i].pos.row == posdir.pos.row &&
            queue[i].pos.col == posdir.pos.col &&
            queue[i].dir == posdir.dir && queue[i].consecutive == posdir.consecutive) {
            return true;
        }
    }
    return false;
}

// For a given position coming from a given direction, we want to know which directions we can go to
// The restrictions are:
// - we can't go back
// - we can't go forward in the same direction more than 3 times
void get_allowed_neighbors(posdir_t current, posdir_t* neighbors, int* nb, int min_consecutive, int max_consecutive) {
    int nb_neighbors = 0;

    char directions[] = { NORTH, EAST, SOUTH, WEST };
    for (int i = 0; i < NB_CARDINAL_DIRECTIONS; i++) {
        direction_t dir = directions[i];

        if ((dir == NORTH && current.dir == SOUTH) ||
            (dir == SOUTH && current.dir == NORTH) ||
            (dir == EAST && current.dir == WEST) ||
            (dir == WEST && current.dir == EAST)) {
            continue; // we don't want to look back
        }

        pos_t direction_delta = convert_dir(dir);

        posdir_t neighbor;
        neighbor.pos.row = current.pos.row + direction_delta.row;
        neighbor.pos.col = current.pos.col + direction_delta.col;

        int row = neighbor.pos.row;
        int col = neighbor.pos.col;

        if (!VALID_COORDS(row, col)) {
            continue;
        }

        // The start position should have no restriction from this point of view (it begins as NONE)
        if (current.dir != NONE) {
            if (current.consecutive < min_consecutive) {
                if (current.dir != dir) {
                    continue;
                }
            } else {
                if (current.dir == dir && current.consecutive >= max_consecutive) {
                    continue;
                }
            }
        }

        neighbor.dir = dir;
        neighbor.consecutive = (neighbor.dir == current.dir) ? current.consecutive + 1 : 1;

        // printf("adding to allowed: (%d,%d,%c,%d)\n", neighbor.pos.row, neighbor.pos.col, neighbor.dir, neighbor.consecutive);
        neighbors[nb_neighbors++] = neighbor;
    }

    *nb = nb_neighbors;
}

posdir_t pop_position_with_least_dist(posdir_t* queue, int* queue_size, int* dist[GRID_SIZE][GRID_SIZE][NB_CARDINAL_DIRECTIONS], int width) {
    int min = INT_MAX;
    int min_idx = 0;
    for (int i = 0; i < *queue_size; i++) {
        posdir_t current = queue[i];
        int dir_idx = dir_to_idx(current.dir);
        for (int j = 0; j < width; ++j) {
            int d = dist[current.pos.row][current.pos.col][dir_idx][j];
            if (d < min) {
                min = d;
                min_idx = i;
            }
        }
    }

    posdir_t res = queue[min_idx];
    for (int i = min_idx; i < *queue_size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    *queue_size -= 1;

    return res;
}

void show_route(int* dist[GRID_SIZE][GRID_SIZE][NB_CARDINAL_DIRECTIONS], int width, posdir_t* path[GRID_SIZE][GRID_SIZE][NB_CARDINAL_DIRECTIONS], pos_t start, pos_t end, int res) {
    posdir_t* route = malloc(sizeof(posdir_t) * 10 * GRID_SIZE);
    int route_size = 0;

    // get prev
    posdir_t cur;
    for (int i = 0; i < NB_CARDINAL_DIRECTIONS; ++i) {
        for (int j = 0; j < width; ++j) {
            if (dist[end.row][end.col][i][j] == res) {
                push(route, (posdir_t) { (pos_t) { end.row, end.col }, .dir = idx_to_dir(i), .consecutive = j }, &route_size);
//                 cur = path[end.row][end.col][i][j];
//                 printf("found dist[%d][%d][%d][%d] = %d\n", end.row, end.col, i, j, res);
                cur = (posdir_t) { (pos_t) { end.row, end.col }, .dir = idx_to_dir(i), .consecutive = j };
                break;
            }
        }
    }
    printf("show route, cur: %d,%d,%c,%d\n", cur.pos.row, cur.pos.col, cur.dir, cur.consecutive);
    printf("DEBUG: %d,%d,%c,%d\n", path[12][12][2][8].pos.row, path[12][12][2][8].pos.col, path[12][12][2][8].dir, path[12][12][2][8].consecutive);

    while (!(cur.pos.row == start.row && cur.pos.col == start.col)) {
        printf("curr: %d,%d,%c,%d, %d\n", cur.pos.row, cur.pos.col, cur.dir, cur.consecutive, dist[cur.pos.row][cur.pos.col][dir_to_idx(cur.dir)][cur.consecutive]);
        posdir_t new_prev = path[cur.pos.row][cur.pos.col][dir_to_idx(cur.dir)][cur.consecutive];
        push(route, cur, &route_size);
        cur = new_prev;
    }

    for (int i = route_size - 1; i > 0; --i) {
        printf("%d: (%d,%d,%c,%d:%d) ", i, route[i].pos.row, route[i].pos.col, route[i].dir, route[i].consecutive, grid[route[i].pos.row][route[i].pos.col]);
    }
    NL;

    char grid_with_route[GRID_SIZE][GRID_SIZE];
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid_with_route[i][j] = '.';
        }
    }

    for (int i = 0; i < route_size; i++) {
        grid_with_route[route[i].pos.row][route[i].pos.col] = route[i].dir;
    }

    printf(" ");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d", i % 10);
    }
    NL;
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d", i % 10);
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c", grid_with_route[i][j]);
        }
        NL;
    }
    NL;

    free(route);
}

int distance(pos_t start, pos_t end, int min_consecutive, int max_consecutive) {
    // For each position on the grid, there can be 4 distances, one for each direction we can come from
    // 0: N, 1: E, 2: S, 3: W
    // For each position, in each direction
    // there can be n distances, n being the max number of consecutive times + 1
    // this allows up to easily index the distances array with the number of consecutive times
    // e.g.: for [min=4,max=10], dist[row][col][card_dir] has 11 elements
    // e.g.: for [min=1,max=3], dist[row][col][card_dir] has 4 elements
    int* dist[GRID_SIZE][GRID_SIZE][NB_CARDINAL_DIRECTIONS];
    posdir_t* path[GRID_SIZE][GRID_SIZE][NB_CARDINAL_DIRECTIONS];
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            for (int k = 0; k < NB_CARDINAL_DIRECTIONS; k++) {
                dist[i][j][k] = malloc(sizeof(int) * (max_consecutive + 1));
                path[i][j][k] = malloc(sizeof(posdir_t) * (max_consecutive + 1));
                for (int l = 0; l < max_consecutive + 1; ++l) {
                    dist[i][j][k][l] = INT_MAX;
                }
            }
        }
    }
    for (int i = 0; i < NB_CARDINAL_DIRECTIONS; i++) {
        // For the starting position the distance is 0
        for (int j = 0; j < max_consecutive + 1; ++j) {
            dist[start.row][start.col][i][j] = 0;
        }
    }

    posdir_t* queue = malloc(sizeof(posdir_t) * QUEUE_SIZE);
    queue[0] = (posdir_t) { .pos = start, .dir = ' ', .consecutive = 0 };
    int queue_size = 1; // initially, we only have the start position in the queue

    typedef struct posdirdist_s {
        posdir_t posdir;
        int dist;
    } posdirdist_t;
    posdirdist_t* seen = malloc(sizeof(posdirdist_t) * QUEUE_SIZE);
    int seen_size = 0;

    // While there are still elements in the queue, we take the first element (i.e. the one with the least distance to the source)
    // and explore its neighbors. If there is a distance with a lower value for a given direction, we update it in distances array
    while (queue_size) {

        // This also was key during my search for a solution, Dijkstra's algorithm requires us to
        // always take the position with the least distance to the source
        posdir_t current = pop_position_with_least_dist(queue, &queue_size, dist, max_consecutive + 1);
        int dist_current = dist[current.pos.row][current.pos.col][dir_to_idx(current.dir)][current.consecutive];

        // we've already seen this position, we don't want to explore it again
        // (another!) auxiliary structure to store the position, direction, and distance
        posdirdist_t posdirdist = (posdirdist_t) {
            .posdir = current,
            .dist = dist_current,
        };
        bool is_seen = false;
        for (int i = 0; i < seen_size; i++) {
            if (seen[i].posdir.pos.row == posdirdist.posdir.pos.row &&
                seen[i].posdir.pos.col == posdirdist.posdir.pos.col &&
                seen[i].posdir.dir == posdirdist.posdir.dir &&
                seen[i].posdir.consecutive == posdirdist.posdir.consecutive &&
                seen[i].dist == posdirdist.dist) {
                is_seen = true;
                break;
            }
        }
        if (is_seen) {
            continue;
        }

        // we've reached the end, we don't want to continue exploring from this point, but we need to process the queue
        // as long as there are elements in it
        if (current.pos.row == end.row && current.pos.col == end.col) {
            continue;
        }

        // If we reached this point, this means we haven't seen this position yet
        // and we will explore its neighbors
        seen[seen_size] = posdirdist;
        seen_size += 1;
        if (seen_size >= QUEUE_SIZE) {
            printf("‼️  seen is full\n");
            exit(1);
        }

        int nb_neighbors = 0;
        posdir_t* neighbors = malloc(sizeof(posdir_t) * NB_CARDINAL_DIRECTIONS);
        get_allowed_neighbors(current, neighbors, &nb_neighbors, min_consecutive, max_consecutive);

        for (int i = 0; i < nb_neighbors; i++) {
            posdir_t neighbor = neighbors[i];
            int row = neighbor.pos.row;
            int col = neighbor.pos.col;
            int dir_idx = dir_to_idx(neighbor.dir);
            int new_distance = dist_current + grid[row][col];

            if (new_distance < dist[row][col][dir_idx][neighbor.consecutive]) {
                dist[row][col][dir_idx][neighbor.consecutive] = new_distance;
                path[row][col][dir_idx][neighbor.consecutive] = current;
            }

            // in all cases we push to the queue, maybe it minimizes the distance
            push(queue, neighbor, &queue_size);
        }

        free(neighbors);
    }

    int res = INT_MAX;
    for (int i = 0; i < NB_CARDINAL_DIRECTIONS ; ++i) {
        int dir = idx_to_dir(i);
        for (int j = min_consecutive; j < max_consecutive + 1; ++j) {
            if (dist[end.row][end.col][i][j] < res) {
                res = dist[end.row][end.col][i][j];
            }
        }
    }

#ifdef TEST
    show_route(dist, max_consecutive + 1, path, start, end, res);
#endif

    free(queue);
    free(seen);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            for (int k = 0; k < NB_CARDINAL_DIRECTIONS; k++) {
                free(path[i][j][k]);
                free(dist[i][j][k]);
            }
        }
    }

    return res;
}

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    char line[1000];
    int lines = 0;
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p) {
            grid[lines][p - line] = (*p - '0');
            p++;
        }

        lines++;
    }

#ifdef TEST
    display_grid();
#endif

    pos_t start = (pos_t) {0, 0};
    pos_t end = (pos_t) {GRID_SIZE - 1, GRID_SIZE - 1};

    int part1 = distance(start, end, 1, 3);
    printf("part1: %d\n", part1);
    int part2 = distance(start, end, 4, 10);
    printf("part2: %d\n", part2);

    fclose(f);
}
