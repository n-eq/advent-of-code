#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CARDS_IN_HAND 5
#define NB_CARD_TYPES 13

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

typedef enum hand_type_e {
    NORMAL = 0,
    PAIR, // 1
    TWO_PAIR, // 2
    THREE_OF_A_KIND, // 3
    FULL_HOUSE, // 4
    FOUR_OF_A_KIND, // 5
    FIVE_OF_A_KIND // 6
} hand_type_t;

typedef struct hand_s {
    char cards[CARDS_IN_HAND];
    int bid;
    struct hand_s* next;
} hand_t;

// reverse order
static int cmpint(const void * a, const void * b) {
   return (*(int*)b - *(int*)a);
}

static int card_score(char card) {
    switch (card) {
        case '2'...'9':
            return card - '0' - 2;
        case 'T':
            return 8;
        case 'J':
            return 9;
        case 'Q':
            return 10;
        case 'K':
            return 11;
        case 'A':
            return 12;
    }
    return 0;
}

// positive if a > b, negative if a < b, 0 if a == b
static int order_cards(const void* a, const void* b) {
   char arg1 = *(const char*)a;
   char arg2 = *(const char*)b;

   return card_score(arg1) - card_score(arg2);
}

// part1: A: 0, K: 1, Q: 2, J: 3, T: 4, 9: 5, 8: 6, 7: 7, 6: 8, 5: 9, 4: 10, 3: 11, 2: 12
// part2: A: 0, K: 1, Q: 2, T: 3, 9: 4, 8: 5, 7: 6, 6: 7, 5: 8, 4: 9, 3: 10, 2: 11, J: 12
static void char_frequency(char* cards, int* freq) {
    memset(freq, 0, NB_CARD_TYPES * sizeof(int)); // just in case the same array is used multiple times
    for (int i = 0; i < CARDS_IN_HAND; i++) {
        int index;
        if (cards[i] == 'A') {
            index = 0;
        } else if (cards[i] == 'K') {
            index = 1;
        } else if (cards[i] == 'Q') {
            index = 2;
        } else if (cards[i] == 'J') {
#ifndef PART2
            index = 3;
#else
            index = 12;
#endif
        } else {
            if (cards[i] == 'T') {
                index = 4;
            } else {
                index = 1 + NB_CARD_TYPES - (cards[i] - '0');
            }
#ifdef PART2
            index--; // shift for part2 
#endif
        }
        freq[index]++;
    }
}

static hand_type_t hand_strength(hand_t* hand) {
    int freq[NB_CARD_TYPES] = {0};
    char_frequency(hand->cards, &freq[0]);

    // Sort frequencies in reverse order (highest first)
    qsort(freq, NB_CARD_TYPES, sizeof(int), cmpint);
    switch (freq[0]) {
        case 5:
            return FIVE_OF_A_KIND;
        case 4:
            return FOUR_OF_A_KIND;
        case 3:
            if (freq[1] == 2) {
                return FULL_HOUSE;
            } else {
                return THREE_OF_A_KIND;
            }
        case 2:
            if (freq[1] == 2) {
                return TWO_PAIR;
            } else {
                return PAIR;
            }
        default:
            return NORMAL;
    }
    return 0;
}

static hand_type_t hand_strength_p2(hand_t* hand) {
    int freq[NB_CARD_TYPES] = {0};
    char_frequency(hand->cards, &freq[0]);

    qsort(freq, NB_CARD_TYPES - 1, sizeof(int), cmpint); // don't sort the jokers

    int jcount = freq[NB_CARD_TYPES - 1];
    hand_type_t strength = hand_strength(hand);
    if (!jcount) {
        return strength;
    }

    switch (freq[0] + jcount) {
    case 5:
        return FIVE_OF_A_KIND;
    case 4:
        return FOUR_OF_A_KIND;
    case 3:
        if (freq[1] == 2) {
            return FULL_HOUSE;
        } else {
            return THREE_OF_A_KIND;
        }
    default:
       if (freq[0] == 2 && (jcount || freq[1] == 2)) {
           return TWO_PAIR;
       }
       if (freq[0] == 2 || jcount) {
           return PAIR;
       }
       return NORMAL;
    }
}

static int cmp_hands(const void* a, const void* b) {
    hand_t* hand_a = (hand_t*) a;
    hand_t* hand_b = (hand_t*) b;

#ifndef PART2
    int strenght_a = hand_strength(hand_a);
    int strenght_b = hand_strength(hand_b);
#else
    int strenght_a = hand_strength_p2(hand_a);
    int strenght_b = hand_strength_p2(hand_b);
#endif

    // If the hands are of different strength, the stronger one wins
    if (strenght_a != strenght_b) {
        return strenght_a - strenght_b;
    }

    // If the hands are of the same strength, the one with the highest card wins
    for (int i = 0; i < CARDS_IN_HAND; i++) {
        if (hand_a->cards[i] != hand_b->cards[i]) {
            return order_cards(&hand_a->cards[i], &hand_b->cards[i]);
        }
    }

    return 0;
}

static void insert_hand_ordered(hand_t** hands, hand_t* hand) {
    hand_t* h = *hands;
    hand_t* prev = NULL;
    while (h) {
        if (cmp_hands(h, hand) > 0) {
            break;
        }
        prev = h;
        h = h->next;
    }

    if (prev) {
        prev->next = hand;
    } else {
        *hands = hand;
    }
    hand->next = h;
}

int main() {
   FILE *fp = fopen(INPUT, "r");
   if (!fp) {
      perror("File opening failed");
      return 1;
   }

   char line[250];

   hand_t* hands = NULL;
   hand_t* h = hands;
   while (fgets(line, sizeof(line), fp)) {
       if (line[0] == '\n') {
           continue;
       }
       line[strcspn(line, "\n")] = 0;

       hand_t* new_hand = malloc(sizeof(hand_t));
       sscanf(line, "%s %d", new_hand->cards, &new_hand->bid);

       insert_hand_ordered(&hands, new_hand);
   }

   h = hands;
   long res = 0;
   for (int i = 1; h; i++, h = h->next) {
       res += h->bid * i;
   }

   printf("res = %ld\n", res);

   // Free-up allocated memory
   h = hands;
   while (h) {
       hand_t* next = h->next;
       if (h) {
           free(h);
       }
       h = next;
   }

   fclose(fp);
}
