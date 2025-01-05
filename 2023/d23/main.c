#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

char** island = NULL;
int island_height = 0;

typedef struct pos_s {
    int x;
    int y;
} pos_t;

static void print_island() {
    for (int i = 0; i < island_height; i++) {
        printf("%s\n", island[i]);
    }
}

typedef pos_t* path_t;
path_t path = NULL;
int longest_hike = 0;

void dfs(pos_t source, pos_t dest, int* current_size, int slope_effect) {
    path = realloc(path, (1 + *current_size) * sizeof(pos_t));
    path[*current_size] = source;
    *current_size = *current_size + 1;

    if (source.x == dest.x && source.y == dest.y) {
        if (*current_size > longest_hike) {
            longest_hike = *current_size;
        }
    }

    int dx[] = { -1, 1, 0, 0 };
    int dy[] = {  0, 0, 1, -1};

    pos_t* next = malloc(sizeof(pos_t));
    pos_t p;
    int next_count = 0;

    char virtual_pos = island[source.y][source.x];

    // small hack to consider slopes as plain '.' positions in part2
    if (!slope_effect && (virtual_pos == '>' || virtual_pos == '<' || virtual_pos == 'v' || virtual_pos == '^')) {
        virtual_pos = '.';
    }

    switch (virtual_pos) {
    case 'v':
        p = (pos_t) { .x = source.x, .y = source.y + 1 };
        next = realloc(next, (1 + next_count) * sizeof(pos_t));
        next[next_count++] = p;
        break;
    case '^':
        p = (pos_t) { .x = source.x, .y = source.y - 1 };
        next = realloc(next, (1 + next_count) * sizeof(pos_t));
        next[next_count++] = p;
        break;
    case '>':
        p = (pos_t) { .x = source.x + 1, .y = source.y};
        next = realloc(next, (1 + next_count) * sizeof(pos_t));
        next[next_count++] = p;
        break;
    case '<':
        p = (pos_t) { .x = source.x - 1, .y = source.y};
        next = realloc(next, (1 + next_count) * sizeof(pos_t));
        next[next_count++] = p;
        break;
    case '.':
        for (int i = 0; i < 4; i++) {
            p = (pos_t) { .x = source.x + dx[i], .y = source.y + dy[i] };
            next = realloc(next, (1 + next_count) * sizeof(pos_t));
            next[next_count++] = p;
        }
        break;
    default:
        break;
    }

    for (int i = 0; i < next_count; i++) {
        pos_t p = next[i];
        if (p.x < 0 || p.x >= island_height || p.y < 0 || p.y >= island_height)
            continue;
        if (island[p.y][p.x] == '#')
            continue;

        int in_path = 0;
        for (int j = 0; j < *current_size; j++) {
            if (path[j].x == next[i].x && path[j].y == next[i].y) {
                in_path = 1;
                break;
            }
        }

        // visiting more than once is not allowed
        if (!in_path)
            dfs(next[i], dest, current_size, slope_effect);
    }
    
    free(next);
    *current_size = *current_size - 1;
}

int find_longest_hike(int slope_effect) {
    pos_t start = { .y = 0 };
    pos_t end = { .y = island_height - 1 };

    for (size_t i = 0; i < strlen(island[0]); i++) {
        if (island[0][i] == '.') {
            start.x = i;
        }
        if (island[island_height - 1][i] == '.') {
            end.x = i;
        }
    }

    int current_size = 0;
    dfs(start, end, &current_size, slope_effect);

    free(path);
    
    return longest_hike - 1;
}

int main() {
    FILE* f = fopen(INPUT, "r");

    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;

        island = realloc(island, (island_height + 1) * sizeof(char*));
        island[island_height] = strdup(line);
        island_height++;
    }
    fclose(f);

    /* print_island(); */
    printf("part1: %d\n", find_longest_hike(1));

    longest_hike = 0;
    path = NULL;
    printf("part2: %d\n", find_longest_hike(0));

    free(island);
}
