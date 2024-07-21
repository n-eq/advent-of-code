#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef EXTRA
#define INPUT "extra"
#else
#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif
#endif

static bool columns_match(char** grid, int lines, int col1, int col2) {
    if (col1 < 0 || col2 < 0 || col1 >= strlen(grid[0]) || col2 >= strlen(grid[0])) {
        return false;
    }

    for (int line = 0; line < lines; line++) {
        if (grid[line][col1] != grid[line][col2]) {
            return false;
        }
    }

    return true;
}

static bool lines_match(char** grid, int lines, int line1, int line2) {
    if (line1 < 0 || line2 < 0 || line1 >= lines || line2 >= lines) {
        return false;
    }

    return (strncmp(grid[line1], grid[line2], strlen(grid[0])) == 0);
}

// Character-wise diff between two lines, -1 if more than one diff, 0 if no diff, 1 if one diff
static int lines_diff(char** grid, int lines, int line1, int line2) {
    if (line1 < 0 || line2 < 0 || line1 >= lines || line2 >= lines) {
        return -1;
    }

    int diff = 0;
    for (int i = 0; i < strlen(grid[0]); i++) {
        if (grid[line1][i] != grid[line2][i]) {
            diff++;
        }
        if (diff > 1) {
            return -1;
        }
    }

    return diff;
}

// A bit crappy and refactorable, but hey...
static long score(char** grid, int lines) {
    int cols = strlen(grid[0]) - 1;

    long res = 0;
    int count_col, count_line;

    bool perfect1 = true;
    bool perfect2 = true;

    count_line = 0;
    for (int line = 0; line < lines - 1; ++line) {
        int i = 0;
        int up;
        while ((up = line - i) >= -1) {
            int down = line + i + 1;

            if (down >= lines || up == -1) {
                // the symmetrical line is out of bounds, if we're here, it means we're got a perfect reflection
                count_line = line + 1;
                perfect1 = true;
                break;
            }

            if (!lines_match(grid, lines, up, down)) {
                perfect1 = false;
                break;
            }

            i++;
        }

        if (perfect1) {
            break;
        }
    }


    count_col = 0;
    for (int col = 0; col < cols - 1; ++col) {
        int i = 0;
        int left;
        while ((left = col - i) >= -1) {
            int right = col + i + 1;

            if (right >= cols || left == -1) {
                // the symmetrical col is out of bounds, if we're here, it means we've got a symmetrical pattern
                perfect2 = true;
                count_col = col + 1;
                break;
            }

            if (!columns_match(grid, lines, left, right)) {
                perfect2 = false;
                break;
            }

            i++;
        }

        if (perfect2) {
            break;
        }
    }

#ifndef PART2
    if (perfect1) {
        res = count_line * 100;
    } else if (perfect2) {
        res = count_col;
    }
#else
    // For part2, we need to exclude the symmetrical lines/columns that form a perfect pattern
    // then find the smudge.
    // To find the smudge, we calculate the diff between the symmetrical lines, 
    // if there is only one diff line-wise, we found the smudge.
    count_line = 0;
    for (int line = 0; line < lines - 1; ++line) {
        if (line == count_line - 1) {
            continue; // skipping the perfect line found in part 1
        }

        int i = 0;
        int up;
        bool found = true;
        while ((up = line - i) >= -1) {
            int down = line + i + 1;

            if (down >= lines || up == -1) {
                // the symmetrical line is out of bounds, if we're here, it means we're got a perfect reflection
                count_line = line + 1;
                found = true;
                break;
            }

            if (lines_diff(grid, lines, up, down) == -1) {
                // too many diffs
                found = false;
                break;
            }

            i++;
        }
        if (found) {
            break;
        }
    }

    return (100 * count_line);
#endif

    return res;
}

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    long res = 0;

    char line[1000];
    int lines = 0;
    char** grid = malloc(sizeof(char*));
    int pattern = 0;
    while (fgets(line, sizeof(line), f)) {
        char* p = line;

#if 0
        if (lines == 0) {
            printf("   ");
            for (int i = 0; i < strlen(p) - 1; i++) {
                printf("%d" , i % 10);
            }
            printf("\n");
        }
#endif

        if (line[0] == '\n')  {
            long tmp = score(grid, lines);
            printf("%d %ld\n", pattern++, tmp);
            res += tmp;

            lines = 0;
            continue;
        }

        grid = realloc(grid, sizeof(char*) * (lines + 1));

        grid[lines] = strdup(p);

#if 0
        printf("%2d %s", lines, p);
#endif

        lines++;
    }

    // This took me hours to debug... I had validated the input example and additional inputs
    // took from redditors, but I just couldn't figure it out...
    // It turned out my parsing considers a grid is finished when it encounters an empty line, which
    // is not the case for the last grid in the file. So this is necessary to check if we're at the end of the file
    // and consider the last grid.
    if (fgets(line, sizeof(line), f) == NULL) {
        long tmp = score(grid, lines);
        res += tmp;
    }

    printf("res: %ld\n", res);

    free(grid);
    fclose(f);
}
