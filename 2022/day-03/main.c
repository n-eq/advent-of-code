// compile & run: gcc main.c && ./a.out
#include <stdbool.h>
#include <ctype.h> // islower, isupper
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if 1
#define INPUT_FILE "input"
#else
#define INPUT_FILE "input_test"
#endif

#define NB_RUCKSACKS 52

/*
 * Normalize a given alphabet character:
 * A..Z -> 0..25
 * a..z -> 26..51
 */
int index_char(c) {
    int cint = (int) c;
    int res = 0;
    if (islower(c)) {
        res = (cint - 97 /* 'a' */) + 26;
    } else {
        res = cint - 65;
    }
    return res;
}

/*
 * Calculate the priority of a given aphabet character
 * according to the challenge's description
 */
int priority(c) {
    if (c >= 'a' && c <= 'z') {
        return (int) c - 'a' + 1;
    } else {
        return (int) c - 'A' + 27;
    }
}

/*
 * Verify if a character is present in the rucksack
 */
bool is_in_rucksack(int* rucksack, char c) {
    return rucksack[index_char(c)] > 0;
}

/* 
 * Displays (for debugging purpopses) a rucksack containing
 * NB_RUCKSACKS integers
 */
void display_rucksack(int* rucksack) {
#if 0
    for (int j = 0; j < NB_RUCKSACKS; ++j) {
        printf("%d ", rucksack[j]);
        if (j == 25) {
            printf("\n\t");
        }
    }
    printf("\n");
#endif
}

void populate_rucksack(int* rucksack, const char* s, size_t l) {
    for (size_t i = 0; i < l; ++i) {
        int index = index_char(s[i]);

        display_rucksack(rucksack);

        /* This is optional (assigining 1 is enough), but I suspected
         * part 2 would involve for some kind of counting occurrences */
        rucksack[index] += 1;

        display_rucksack(rucksack);
    }
}

void part1() {
    char buf[64];

    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("Opening input file");
        exit(1);
    }

    long sum = 0;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        /* strip trailing newline character */
        buf[strcspn(buf, "\n")] = 0;

        size_t l = strlen(buf);
        size_t half = l / 2;

        char* left = malloc((half + 1) * sizeof(char));
        char* right = malloc((half + 1) * sizeof(char));

        if (memcpy(left, buf, sizeof(char) * half) == NULL) {
            perror("Failed to copy left part");
            exit(2);
        }
        /* nullify the last char to make it a valid C string */
        left[half] = '\0';

        if (memcpy(right, buf + half, sizeof(char) * half) == NULL) {
            perror("Failed to copy right part");
            exit(2);
        }
        /* nullify the last char to make it a valid C string */
        right[half] = '\0';

        /* double array of NB_RUCKSACKS (26 + 26) 'a..zA..Z' representing the presence of a given
         * letter in the left (resp. right) array */
        int rucksack[2][NB_RUCKSACKS] = {{0}};

        populate_rucksack((int *) rucksack, left, half);
        populate_rucksack((int *) (rucksack + 1), right, half);

        for (int i = 0; i < NB_RUCKSACKS; ++i) {
            char c;
            if (i < 26) {
                c = (i + 65);
            } else {
                c = (i - 26) + 'a';
            }

            if (is_in_rucksack(rucksack[0], c) && is_in_rucksack(rucksack[1], c)) {
                sum += priority(c);
            }
        }

        free(left);
        free(right);
    }
    printf("part1: %ld\n", sum);

    fclose(fp);
}

void part2() {
    char buf[64];

    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("Opening input file");
        exit(1);
    }

    long sum = 0;
    int i = 0;
    int rucksacks[3][NB_RUCKSACKS] = {{0}};
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        /* strip trailing newline character */
        buf[strcspn(buf, "\n")] = 0;

        size_t l = strlen(buf);

        populate_rucksack((int *) &rucksacks[i], buf, l);

        /* check and reset the ruckstacks for the next group */
        if (i++ == 2) {
            i = 0;

            // find the common char to the three rucksacks
            for (int j = 0; j < NB_RUCKSACKS; ++j) {
                char c;
                if (j > 25) {
                    c = (j - 26) + 'a';
                } else {
                    c = (j + 65);
                }

                if (is_in_rucksack(rucksacks[0], c) && is_in_rucksack(rucksacks[1], c) && is_in_rucksack(rucksacks[2], c)) {
                    sum += priority(c);
                }
            }

            /* Reset the arrays for the next interation */
            memset(rucksacks, 0, sizeof(int) * NB_RUCKSACKS * 3);
        }
    }

    printf("part2: %ld\n", sum);
    fclose(fp);
}

int main() {

    part1();
    part2();
}
