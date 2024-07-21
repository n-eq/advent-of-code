#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

int main() {
    FILE* f = fopen("input", "r");
    if (f == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    int possible_games = 0;
    int game = 1;

    long res_part2 = 0;

    char line[500];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\0') {
            continue;
        }
        // strip trailing newline character
        line[strcspn(line, "\n")] = 0;
        size_t len = strlen(line);
        line[len] = ';';
        line[len + 1] = '\0';

        // NEW GAME
        char* p = strchr(line, ':') + 2;
        // printf("line = %s\n", p);

        int remaining[/* red, green, blue */] = { 12, 13, 14 };
        
        // Shortcut to skip the game (double loop) if it's not possible
        bool shortcut = false;

        int min[3] = { 0 };
        char* q = strtok(p, ";");
        while (q) {
            while (1) {
                char color[10];
                int count;
                sscanf(q, "%d %s", &count, color);

                // For some reason sscanf also takes the comma (,) as part of the color
                // (even when explicitly specifying '%s,'), so we need to remove it
                if (strchr(color, ',')) {
                    *strchr(color, ',') = '\0';
                }

                if (strncmp(color, "red", 3) == 0) {
                    remaining[0] -= count;
                    if (count > min[0]) {
                        min[0] = count;
                    }
                } else if (strncmp(color, "green", 5) == 0) {
                    remaining[1] -= count;
                    if (count > min[1]) {
                        min[1] = count;
                    }
                } else if (strncmp(color, "blue", 4) == 0) {
                    remaining[2] -= count;
                    if (count > min[2]) {
                        min[2] = count;
                    }
                }

                // For part 2 we shouldn't break the loop if we find a negative value
                // because we need to find the highest value for each color
#ifndef PART2
                if (remaining[0] < 0 || remaining[1] < 0 || remaining[2] < 0) {
                    shortcut = true;
                    break;
                }
#endif

                char* tmp = strchr(q, ',');
                if (tmp) {
                    q = tmp + 2;
                } else {
                    // NEW SET
                    break;
                }
            }

            if (shortcut) {
                break;
            }


            q = strtok(NULL, ";");
            remaining[0] = 12;
            remaining[1] = 13;
            remaining[2] = 14;
        }

        if ((remaining[0] > 0) && (remaining[1] > 0) && (remaining[2] > 0)) {
            possible_games += game;
        }

#ifdef PART2
        res_part2 += min[0] * min[1] * min[2];
#endif


        game++;
    }

    printf("Possible games: %d\n", possible_games);
#ifdef PART2
    printf("Part 2 result: %ld\n", res_part2);
#endif

    fclose(f);
}
