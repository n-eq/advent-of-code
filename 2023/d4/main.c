#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define WINNING_COUNT 10
#define NB_CARDS 220

typedef struct card_s {
    int id;
    int nb_winning;
    int multiplier;
} card_t;

static card_t cards[NB_CARDS];

int main() {
    FILE *file = fopen("input", "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return 1;
    }

    for (int i = 0; i < NB_CARDS; ++i) {
        cards[i].id = i;
        cards[i].nb_winning = 0;
        cards[i].multiplier = 1;
    }

    char line[256];
    int winning[WINNING_COUNT] = {0};
    int total_p1 = 0;
    int total_p2 = 0;
    int card_copy_count[NB_CARDS] = { 0 };
    int card_i = 0;
    while (fgets(line, sizeof(line), file)) {
        printf("line: %s\n", line);

        int ours_winning[15] = {0};
        int winning_count = 0;

        // Split the line into two parts
        char* p = strchr(line, ':') + 1;
        char *winning_numbers = strtok(p, "|") + 1;
        char *your_numbers = strtok(NULL, "|");

        // Split the winning numbers into individual numbers
        char *winning_number = strtok(winning_numbers, " ");
        int i = 0;
        while (winning_number != NULL) {
            winning[i++] = atoi(winning_number);
            winning_number = strtok(NULL, " ");
        }

        // Parse and check input numbers (right hand side of the line)
        char* your_number = strtok(your_numbers, " ");
        while (your_number != NULL) {
            int number = atoi(your_number);
            for (int i = 0; i < WINNING_COUNT; i++) {
                if (number == winning[i]) {
                    printf("Number %d is a winning number\n", number);
                    winning_count++;
                    cards[card_i].nb_winning++;
                    break;

                }
            }
            your_number = strtok(NULL, " ");
        }

        if (winning_count > 0) {
            total_p1 += pow(2, winning_count - 1);
        }

        while (winning_count > 0) {
            for (int k = 0; k < cards[card_i].multiplier; ++k) {
                cards[card_i + winning_count].multiplier++;
            }
            winning_count--;
        }
        total_p2 += cards[card_i].multiplier;

        i++;
        card_i++;
    }

    printf("Total: p1: %d, p2: %d\n", total_p1, total_p2);

    fclose(file);
    return 0;
}
