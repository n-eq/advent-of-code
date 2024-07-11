#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NB_BOXES 256
#define LABEL_MAX_SIZE 10

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

typedef struct slot_s {
    const char* label;
    int focal_length;
    struct slot_s* next;
} slot_t;

static slot_t* boxes[NB_BOXES];

void display_boxes() {
    for (int i = 0; i < NB_BOXES; ++i) {
        if (!boxes[i]) {
            continue;
        }

        slot_t* s = boxes[i];
        printf("box %d ", i);
        while (s) {
            printf("[%s %d] ", s->label, s->focal_length);
            s = s->next;
        }
        printf("\n");
    }
}

void free_boxes() {
    for (int i = 0; i < NB_BOXES; ++i) {
        if (!boxes[i]) {
            continue;
        }

        slot_t* s = boxes[i];
        while (s) {
            slot_t* tmp = s;
            s = s->next;
            free(tmp);
        }
    }
}

void hash_str(int* current, const char* str) {
    while (*str) {
        int c = *str;
        *current += (int) c;
        *current *= 17;
        *current %= 256;
        str++;
    }
}

void insert_slot(char* s) {
    char* label = malloc(LABEL_MAX_SIZE * sizeof(char));
    char op;
    int focal_length;

    char* s2;
    if ((s2 = strchr(s, '-'))) {
        int i = 0;
        while (s != s2) {
            label[i++] = *s++;
        }
        label[i] = '\0';

        int hash = 0;
        hash_str(&hash, label);

        slot_t* target = boxes[hash];
        for (int i = 0; i < NB_BOXES; ++i) {
            if (!boxes[i]) {
                continue;
            }

            // we need to remove all slots with the same label
            slot_t* prev = NULL;
            target = boxes[i];
            while (target) {
                if (strcmp(label, target->label) == 0) {
                    if (prev) {
                        prev->next = target->next;
                    } else {
                        boxes[i] = target->next;
                    }
                    free(target);

                    // there can be at most one per box
                    break;
                }
                prev = target;
                target = target->next;
            }
        }
    } else if ((s2 = strchr(s, '='))) {
        int i = 0;
        while (s != s2) {
            label[i++] = *s++;
        }
        label[i] = '\0';

        focal_length = atoi(s2 + 1);

        int hash = 0;
        hash_str(&hash, label);

        slot_t slot = {
            .label = label,
            .focal_length = focal_length
        };

        slot_t* target = boxes[hash];

        if (boxes[hash]) {
            // if there's already a lense in this box with the same label, replace it
            bool replaced = false;

            while (target) {
                if (strcmp(target->label, label) == 0) {
                    // we just need to change focal_length value
                    target->focal_length = focal_length;
                    replaced = true;
                    break;
                }
                target = target->next;
            }
            if (!replaced) {
                target = boxes[hash];
                while (target->next) {
                    target = target->next;
                }

                // otherwise, add it to the end of the list
                slot_t* new_slot = malloc(sizeof(slot_t));
                *new_slot = slot;
                new_slot->next = NULL;
                target->next = new_slot;
            }
        } else {
            // otherwise, add it to the end of the list
            slot_t* new_slot = malloc(sizeof(slot_t));
            *new_slot = slot;
            new_slot->next = NULL;

            boxes[hash] = new_slot;
        }
    }
}

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    int res = 0;

    char line[30000];

#ifdef PART2
    for (int i = 0;  i < NB_BOXES; ++i) {
        boxes[i] = NULL;
    }
#endif

    while (fgets(line, sizeof(line), f)) {
        char* p = line;

        char* q;
        do {
            // The stop character should either be a comma or a newline
            // If neither is detected, we are at the end of the string
            q = strchr(p, ',');
            if (!q) {
                q = strchr(p, '\n');
            }
            if (!q) {
                break;
            }
            *q = '\0';

#ifndef PART2
            int hash = 0;
            hash_str(&hash, p);
            res += hash;
#else
            insert_slot(p);
#endif

            p = q + 1;
        } while (p);
    }

#ifdef PART2
    for (int i = 0; i < NB_BOXES; ++i) {
        if (!boxes[i]) {
            continue;
        }

        slot_t* s = boxes[i];
        int j = 1;
        while (s) {
            int tmp = (i + 1) * j * s->focal_length;
            res += tmp;

            j++;
            s = s->next;
        }
    }
#endif

    printf("res: %d\n", res);

    free_boxes();
    fclose(f);
}
