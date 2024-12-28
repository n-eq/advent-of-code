#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define WORD_COUNT 14

static int same(const char* s1, const char* s2) {
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);
    if (l1 != l2) {
        return 0;
    }

    for (size_t i = 0; i < l1; i++) {
        int found = 0;
        for (size_t j = 0; j < l2; j++) {
            if (s1[i] == s2[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            return 0;
        }
    }
    return 1;
}

static int diff_count(char word[], int len, char mappings[], int count) {
    int diff = 0;
    for (int i = 0; i < len; i++) {
        char c = word[i];
        int found = 0;
        for (int j = 0; j < count; j++) {
            if (mappings[j] == c) {
                found = 1;
                break;
            }
        }
        if (!found) {
            diff++;
        }
    }
    return diff;
}

static unsigned int solve(char** words) {
    char** mapping = (char** ) malloc(10 * sizeof(int*));
    for (int i = 0; i < 10; i++) {
        mapping[i] = (char* ) malloc(7 * sizeof(int));
        memset(mapping[i], ' ', 7 * sizeof(int));
    }
    int mapping_count[10] = { 0 };

    char** words_sorted_by_len = malloc(WORD_COUNT * sizeof(char*));
    for (int i = 0; i <WORD_COUNT; i++) {
        words_sorted_by_len[i] = strdup(words[i]);
    }
    char temp[9];
    for(int i=0; i<WORD_COUNT - 1;i++)
    {
        for (int j = 0; j < WORD_COUNT - i - 1; j++) {
            if(strlen(words_sorted_by_len[j])> strlen(words_sorted_by_len[j+1])) {
                strcpy(temp,words_sorted_by_len[j]);
                strcpy(words_sorted_by_len[j],words_sorted_by_len[j+1]);
                strcpy(words_sorted_by_len[j+1],temp);
            }
        }
    }

    for (int i = 0; i < WORD_COUNT; i++) {
        char* w = words_sorted_by_len[i];
        size_t len = strlen(w);

        switch (len) {
        // ONE
        case 2:
            memcpy(mapping[1], w, len);
            mapping_count[1] = len;
            break;
        // SEVEN
        case 3:
            memcpy(mapping[7], w, len);
            mapping_count[7] = len;
            break;
        // FOUR
        case 4:
            memcpy(mapping[4], w, len);
            mapping_count[4] = len;
            break;
        // EIGHT
        case 7:
            memcpy(mapping[8], w, len);
            mapping_count[8] = len;
            break;
        // TWO, THREE, FIVE
        case 5:
            if (diff_count(mapping[1], mapping_count[1], w, len) == 0) {
                memcpy(mapping[3], w, len * sizeof(char));
                mapping_count[3] = len;
            } else if (diff_count(w, len, mapping[4], mapping_count[4]) == 2) {
                memcpy(mapping[5], w, len * sizeof(char));
                mapping_count[5] = len;
            } else if (diff_count(w, len, mapping[4], mapping_count[4]) == 3) {
                memcpy(mapping[2], w, len * sizeof(char));
                mapping_count[2] = len;
            }
            break;
        // ZERO, SIX, NINE
        case 6:
            if (diff_count(mapping[1], mapping_count[1], w, len) == 1) {
                memcpy(mapping[6], w, len * sizeof(char));
                mapping_count[6] = len;
            } else if (diff_count(mapping[5], mapping_count[5], w, len) == 0) {
                memcpy(mapping[9], w, len * sizeof(char));
                mapping_count[9] = len;
            } else {
                memcpy(mapping[0], w, len * sizeof(char));
                mapping_count[0] = len;
            }
            break;
        default:
            break;
        }

    }

#if 0
    for (int r = 0; r < 10; r++) {
        char* mappings = mapping[r];
        if (mapping_count[r]) {
            printf("mappings %d: ", r);
            for (int j = 0; j < mapping_count[r]; j++) {
                printf("%c ", mappings[j]);
            }
            printf("\n");
        }
    }
    printf("////\n");
#endif

    int res = 0;
    for (int i = 0; i < 4; i++) {
        char* w = words[WORD_COUNT - 1 - i];
        for (int j = 0; j < 10; j++) {
            char* pattern = mapping[j];
            pattern[mapping_count[j]] = '\0';
            if (same(w, pattern)) {
                res += j * pow(10, i);
                break;
            }
        }
    }

    for (int i = 0; i < 10; i++) {
        free(words_sorted_by_len[i]);
        free(mapping[i]);
    }
    free(words_sorted_by_len);
    free(mapping);
    return res;
}

int main() {
    FILE *file = fopen("input", "r");

    char* line = NULL;
    char** words = (char**) malloc(WORD_COUNT * sizeof(char*));

    unsigned int part1 = 0;
    unsigned int part2 = 0;

    size_t len;
    while (getline(&line, &len, file) != -1) {
        // strip trailing newline from the nend
        line[strcspn(line, "\n")] = 0;

        int nb_words = 0;

        char* w = strtok(line, " ");
        while (w) {
            if (w[0] != '|') {
                words[nb_words++] = strdup(w);

                // For part1, we only need to consider the 4 last words)
                if (nb_words > 10) {
                    int slen = strlen(w);
                    switch (slen) {
                        case 2:
                        case 3:
                        case 4:
                        case 7:
                            part1++;
                            break;
                        default:
                            break;
                    }
                }
            }

            w = strtok(NULL, " ");
        }

        part2 += solve(words);
    }


    printf("part1: %d\n", part1);
    printf("part2: %d\n", part2);

    if (line) {
        free(line);
    }
    for (int i = 0; i < WORD_COUNT; i++) {
        free(words[i]);
    }
    free(words);

    fclose(file);
    return 0;
}
