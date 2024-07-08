/*
 * Funny story, for this day I got stuck for more than hour before completing part 1 for a very dumb reason.
 * My code was working for the test input but not the real input. After rewriting the tilt algorithm more
 * than once I found out (by accident) that the input I copied from the AoC was weird, it had lines of different sizes.
 * It took me a while before realizing that Google was automatically translating (from portuguse, who knows why?!) the input
 * and thus changing the data...
 * Another day with input-relateed issues, rrrr.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

#define NB_CYCLES 1000000000

static inline void display(char** lines, int nb_lines) {
    for (int i = 0; i < nb_lines; i++) {
        printf("%s", lines[i]);
    }
}

long get_load(char** lines, int nb_lines) {
    long res = 0;
    for (int i = 0; i < nb_lines; i++) {
        long rock_count = 0;
        for (int j = 0; j < nb_lines; j++) {
            if (lines[i][j] == 'O') {
                rock_count++;
            }
        }
        res += rock_count * (nb_lines - i);;
    }

    return res;
}

void tilt(char dir, char** lines, int nb_lines) {
    size_t len = strlen(lines[0]);
    if (dir == 'N') {
        for (int line = 1; line < nb_lines; ++line) {
            for (int col = 0; col < len; ++col) {
                if (lines[line][col] == 'O') {
                    int j = line - 1;
                    while (j >= 0 && lines[j][col] == '.') {
                        j--;
                    }

                    if (line != j + 1) {
                        lines[j + 1][col] = 'O';
                        lines[line][col] = '.';
                    }
                }
            }
        }
    } else if (dir == 'S') {
        for (int line = nb_lines - 1; line >= 0; line--) {
            for (int col = 0; col < len; ++col) {
                if (lines[line][col] == 'O') {
                    int j = line + 1;
                    while (j < nb_lines && lines[j][col] == '.') {
                        j++;
                    }

                    if (line != j - 1) {
                        lines[j - 1][col] = 'O';
                        lines[line][col] = '.';
                    }
                }
            }
        }
    } else if (dir == 'W') {
        for (int line = 0; line < nb_lines; ++line) {
            for (int col = 1; col < len; ++col) {
                if (lines[line][col] == 'O') {
                    int new_col = col - 1;
                    while (new_col >= 0 && lines[line][new_col] == '.') {
                        new_col--;
                    }

                    if (col != new_col + 1) {
                        lines[line][new_col + 1] = 'O';
                        lines[line][col] = '.';
                    }
                }
            }
        }
    } else if (dir == 'E') {
        for (int line = 0; line < nb_lines; ++line) {
            for (int col = len - 1; col >= 0; col--) {
                if (lines[line][col] == 'O') {
                    int new_col = col + 1;
                    while (new_col <= len && lines[line][new_col] == '.') {
                        new_col++;
                    }

                    if (col != new_col - 1) {
                        lines[line][new_col - 1] = 'O';
                        lines[line][col] = '.';
                    }
                }
            }
        }
    }
}

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    long res = 0;

    char tmp[1000];
    char* lines[1000];
    int nb_lines = 0;
    while (fgets(tmp, sizeof(tmp), f)) {
        lines[nb_lines] = malloc(strlen(tmp) + 1);
        strcpy(lines[nb_lines], tmp);

        nb_lines++;
    }

#ifndef PART2
    tilt('N', lines, nb_lines);
#else
    long prev = 0;
    for (int i = 0; i < NB_CYCLES; ++i) {
        for (int j = 0; j < 4; ++j) {
            tilt((char[] ){ 'N', 'W', 'S', 'E' }[j], lines, nb_lines);  
        }
        // To be honest, for part 2 I got lucky.
        // Seeing the results I quickly realized brute-forcing would take dozens of hours.
        // I plotted the first 1000ish lines and it seems the scatter converges quickly to the final result,
        // oscillating between a few other values...
        // I took a chance and tried a few ones, and it turned out the result was one of the values in the cycles
        // (for my input it appeared twice in a row...)
        // For the real solution, you'll need to have an array where you store the N last values and detect a cycle
        // in the load results.
    }

#endif

    printf("res = %ld\n", res);

    for (int i = 0; i < nb_lines; i++) {
        free(lines[i]);
    }
    fclose(f);
}
