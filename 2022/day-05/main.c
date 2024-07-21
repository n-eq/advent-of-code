// compile & run:
// part1: gcc main.c -DPART1 & ./a.out
// part2: gcc main.c & ./a.out
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // isalpha
#include <stdio.h>

#define INPUT_FILE "input"

// uncomment to do part2
// #define PART1

typedef struct crate_s {
    char name;
    struct crate_s* next;
} crate_t;

/*
 * List of stacks. At this point, we don't know yet the number of stacks
 * we are dealing with, so we initialize it as a pointer to a pointer to a Crate
 * (~ double list of Crates)
 */
static crate_t** stacks;
static size_t nb_stacks;
    
static void free_stacks() {
    for (size_t i = 0; i < nb_stacks; ++i) {
        free(*(stacks + i));
        *(stacks + i) = NULL;
    }

    free(stacks);
    stacks = NULL;
}

static size_t stack_len(crate_t* stack) {
    size_t res = 0;
    while (stack) {
        res++;
        stack = stack->next;
    }
    return res;
}

/*
 * Push a crate to the end of a stack 
 */
void push_to_stack(crate_t** stack, char c) {
    crate_t* new_crate = malloc(sizeof(crate_t));
    new_crate->next = NULL;
    new_crate->name = c;

    crate_t* s = *stack;
    while (s->next) {
        s = s->next;
    }

    s->next = new_crate;
}

/*
 * Add a crate to the beginning of a stack
 */
void add_to_head(crate_t** stack, char c) {
    if (*stack == NULL) {
        *stack = malloc(sizeof(crate_t));
        (*stack)->name = c;
        (*stack)->next = NULL;
        
        return;
    }

    crate_t* new_head = malloc(sizeof(crate_t));
    new_head->name = c;
    new_head->next = (*stack);

    *stack = new_head;
}

/*
 * Get and remove the last crate in a stack
 */
char pop_stack(crate_t** stack) {
    if (!*stack) {
        return ' ';
    }

    crate_t* c1 = *stack;
    crate_t* c2 = *stack; // follows c1 with one step late

    while (c1->next) {
        c2 = c1;
        c1 = c1->next;
    }
    c2->next = NULL;

    return c1->name;
}

int main(void) {
    char buf[64];

    FILE *fp = fopen(INPUT_FILE, "r");
    if (fp == NULL) {
        perror("Opening input file");
        exit(1);
    }

    /* We use this variable to determine (and allocate!) the number of stacks we have, once at the first line. */
    size_t iteration = 0;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        /* strip trailing newline character */
        buf[strcspn(buf, "\n")] = 0;

        if (iteration == 0) {
            nb_stacks = strlen(buf) / 3;
            stacks = malloc(sizeof(crate_t* ) * nb_stacks);
            
            for (int j = 0; j < nb_stacks; ++j) {
                *(stacks + j) = NULL;
            }
        }


        /* if the line contains '[', it's a stage of stacks to parse,
         * if it contains 'move', it's an instruction to execute,
         * otherwise, just skip */
        if (strchr(buf, '[') != NULL) {
            // [c] [c] [c] [ c ] [c]
            // 01   5   9   13...
            for (int i = 0; i < nb_stacks; ++i) {
                char c = buf[i * 4 + 1];
                if (isalpha(c)) {
                    add_to_head(stacks + i, c);
                }
            }
        } else if (NULL != strstr(buf, (const char* ) "move")) {
            int source, dest, count;
            sscanf(buf, "move %d from %d to %d", &count, &source, &dest);

            /* normalize indexes (the input starts at 1, our list stacks starts at 0) */
            source--;
            dest--;

#ifdef PART1
            for (int i = 0; i < count; ++i) {
                char c = pop_stack(stacks + source);
                push_to_stack(stacks + dest, c);
            }
#else
            /* 
             * For part2, we could implement a function to extract last n crates in
             * a stack and another one to extend a crate some other crate but I think
             * it's too much error-prone code to write. Instead, just push all the popped
             * crates in a string (list of chars), read it in reverse and call push_to_stack
             * as we did in part1.
             */

            // /!\ don't try to printf (%s) this variable (it's not null-terminated)!
            char* chars_to_move = malloc(sizeof(char) * count);
            for (size_t i = 0; i < count; ++i) {
                chars_to_move[i] = pop_stack(stacks + source);
            }

            /* now loop in reverse */
            for (size_t i = 0; i < count; ++i) {
                push_to_stack(stacks + dest, chars_to_move[count - i - 1]);
            }

            free(chars_to_move);
            chars_to_move = NULL;
#endif
        }

        iteration++;
    }

    char* result = malloc(sizeof(char) * (nb_stacks + 1));
    for (size_t i = 0; i < nb_stacks; ++i) {
        result[i] = pop_stack(stacks + i);
    }
    result[nb_stacks] = '\0'; /* let's make sure we have a valid C string */

#ifdef PART1
    printf("part1: %s\n", result);
#else
    printf("part2: %s\n", result);
#endif

    free_stacks();
    free(result);
    result = NULL;


    return 0;
}
