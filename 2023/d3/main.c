#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

#ifdef PART2
typedef struct gear_s {
    int adjacent_pn[6]; // We can have at most 6 adjacent part numbers
    int nb_adjacent_pn;
} gear_t;

// We store a 2D array corresponding to the input lines, only coordinates which have a gear '*'
// are useful
static gear_t gears[150][150] = {0};
#endif

int main() {
    FILE* f = fopen("input", "r");

    char line[150];
    int res_p1 = 0;
    int res_p2 = 0;

    // first we collect all the lines in an array, it's easier in order to iterate on multiple lines at once
    int nb_lines = 0;
    char** lines = malloc(150 * sizeof(char* ));
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\0') {
            continue;
        }
        // strip trailing newline character
        line[strcspn(line, "\n")] = 0;

        lines[nb_lines] = malloc(strlen(line) + 1);
        strcpy(lines[nb_lines++], line);
    }

    for (int line = 0; line < nb_lines; ++line) {
        char* p = lines[line];
        int i = 0;
        while (i  < strlen(p)) {
            if (isdigit(p[i])) {
                char* q;
                int n = strtol(p + i, &q, 10);
                int nb_digits = q - (p + i);

                debug("new number: %d, i = %d, nb digits: %d\n", n, i, nb_digits);


                // look for any adjacent symbol other than '.' in the line above and below
                for (int k = 0; k < nb_digits; k++) {
                    if (
                            (line >= 1 && lines[line - 1][i + k] != '.') ||
                            (line + 1 < nb_lines  && lines[line + 1][i + k] != '.')
                       ) {
                        res_p1 += n;
                        debug("found %d in up or down (%c %c, %d)\n", n, lines[line - 1][i + k], lines[line + 1][i + k], i + k);


#ifdef PART1
                        // For part1, we can break here, as we only need to find one adjacent symbol
                        break;
#endif

#ifdef PART2
                        // For part2 however, we must check all adjacent gears
                        if (line >= 1) {
                            if (lines[line - 1][i + k] == '*') {
                                gears[line - 1][i + k].adjacent_pn[gears[line - 1][i + k].nb_adjacent_pn++] = n;
                            }
                        }

                        if (line + 1 < nb_lines) {
                            if (lines[line + 1][i + k] == '*') {
                                gears[line + 1][i + k].adjacent_pn[gears[line + 1][i + k].nb_adjacent_pn++] = n;
                            }
                        }
#endif
                    }
                }

                // look for any adjacent symbol other than '.' in left hand side
                if (i > 0) {
                    if (
                            (line >= 1 && lines[line - 1][i - 1] != '.') ||
                            (lines[line][i - 1] != '.') ||
                            (line + 1 < nb_lines && lines[line + 1][i - 1] != '.')
                       ) {
                        res_p1 += n;
                        debug("found %d in left side\n", n);

#ifdef PART2
                        if (line >= 1) {
                            if (lines[line - 1][i - 1] == '*') {
                                gears[line - 1][i - 1].adjacent_pn[gears[line - 1][i - 1].nb_adjacent_pn++] = n;
                            }
                        }

                        if (line + 1 < nb_lines) {
                            if (lines[line + 1][i - 1] == '*') {
                                gears[line + 1][i - 1].adjacent_pn[gears[line + 1][i - 1].nb_adjacent_pn++] = n;
                            }
                        }

                        if (lines[line][i - 1] == '*') {
                            gears[line][i - 1].adjacent_pn[gears[line][i - 1].nb_adjacent_pn++] = n;
                        }
#endif

                    }
                }

                // look for any adjacent symbol other than '.' in right hand side
                if (i + nb_digits < strlen(p) - 1) {
                    if (
                            (line >= 1 && lines[line - 1][i + nb_digits] != '.') ||
                            (lines[line][i + nb_digits] != '.') ||
                            (line + 1 < nb_lines && lines[line + 1][i + nb_digits] != '.')
                       ) {
                        res_p1 += n;
                        debug("found %d in right side\n", n);

#ifdef PART2
                        if (line >= 1) {
                            if (lines[line - 1][i + nb_digits] == '*') {
                                gears[line - 1][i + nb_digits].adjacent_pn[gears[line - 1][i + nb_digits].nb_adjacent_pn++] = n;
                            }
                        }

                        if (line + 1 < nb_lines) {
                            if (lines[line + 1][i + nb_digits] == '*') {
                                gears[line + 1][i + nb_digits].adjacent_pn[gears[line + 1][i + nb_digits].nb_adjacent_pn++] = n;
                            }
                        }

                        if (lines[line][i + nb_digits] == '*') {
                            gears[line][i + nb_digits].adjacent_pn[gears[line][i + nb_digits].nb_adjacent_pn++] = n;
                        }
#endif

                    }
                }
                i += nb_digits;
            } else {
                i++;
            }
        }
    }


#ifdef PART1
    printf("part1: %d\n", res_p1);
#endif

#ifdef PART2 
    for (int i = 0; i < 150; ++i) {
        for (int j = 0; j < 150; ++j) {
            if (gears[i][j].nb_adjacent_pn == 0) {
                continue;
            }
            if (gears[i][j].nb_adjacent_pn == 2) {
                debug("gear[%d][%d]: ", i, j);
                for (int k = 0; k < gears[i][j].nb_adjacent_pn; ++k) {
                    debug("%d ", gears[i][j].adjacent_pn[k]);
                }
                debug("\n");
                res_p2 += gears[i][j].adjacent_pn[0] * gears[i][j].adjacent_pn[1];
            }
        }
    }
    printf("part2: %d\n", res_p2);
#endif

    free(lines);
    fclose(f);
}
