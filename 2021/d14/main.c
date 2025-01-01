#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_CHARACTERS 26
#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

typedef struct rule_s {
    char pattern[2];
    char result;
} rule_t;

static char* polymer = NULL;
static rule_t* rules = NULL;
static int nb_rules = 0;

unsigned long long int score(unsigned long long int* occurrences) {
    unsigned long long int min, max;
    min = ULLONG_MAX;
    max = 0;

    for (int i = 0; i < NB_CHARACTERS; i++) {
        if (occurrences[i] > max) {
            max = occurrences[i];
        }
        if (occurrences[i] > 0 && occurrences[i] < min) {
            min = occurrences[i];
        }
    }

    return max - min;
}

int main() {
    FILE* f = fopen(INPUT, "r");

    int lines = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';

        if (lines == 0) {
            polymer = strdup(line);
        } else if (line[0] != '\0') {
            char a, b, c;
            if (sscanf(line, "%c%c -> %c", &a, &b, &c) != 3) {
                break;
            }

            rules = realloc(rules, (nb_rules + 1) * sizeof(rule_t));
            rules[nb_rules++] = (rule_t) {
                .pattern = { a, b },
                    .result = c
            };
        }

        lines++;
    }

    int max_pairs = NB_CHARACTERS;
    unsigned long long int occurrences[NB_CHARACTERS] = {0};
    memset(occurrences, 0, NB_CHARACTERS * sizeof(unsigned long long int));

    unsigned long long int pairs[max_pairs][max_pairs];
    for (int i = 0; i < max_pairs; i++) {
        memset(pairs[i], 0, max_pairs * sizeof(unsigned long long int));
    }
    for (int i = 0; isalpha(polymer[i]); i++) {
        occurrences[polymer[i] - 'A']++;
    }
    for (int i = 0; isalpha(polymer[i + 1]); i++) {
        pairs[polymer[i] - 'A'][polymer[i + 1] - 'A']++;
    }

    int nb_steps = 40;
    unsigned long long int new_pairs[max_pairs][max_pairs];
    for (int step = 0; step < nb_steps; step++) {
        if (step == 10) {
            printf("part1: %lld\n", score(occurrences));
        }
        memcpy(new_pairs, pairs, max_pairs * max_pairs * sizeof(unsigned long long int));
        for (int r = 0; r < nb_rules; r++) {
            int c1 = rules[r].pattern[0] - 'A';
            int c2 = rules[r].pattern[1] - 'A';
            int c3 = rules[r].result - 'A';

            new_pairs[c1][c3] += pairs[c1][c2];
            new_pairs[c3][c2] += pairs[c1][c2];
            new_pairs[c1][c2] -= pairs[c1][c2];
            occurrences[c3] += pairs[c1][c2];
        }
        memcpy(pairs, new_pairs, max_pairs * max_pairs * sizeof(unsigned long long int));
    }
    printf("part2: %lld\n", score(occurrences));

    if (polymer) {
        free(polymer);
    }
    if (rules) {
        free(rules);
    }
    fclose(f);
}
