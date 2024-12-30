#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

#define CAPACITY 128

typedef struct stack_s {
    char arr[CAPACITY];  
    int top;        
} stack_t;

void initialize(stack_t* stack) {
    stack->top = -1;  
}

bool isEmpty(stack_t* stack) {
    return stack->top == -1;  
}

bool isFull(stack_t* stack) {
    return stack->top == CAPACITY - 1;  
}

void push(stack_t* stack, char value) {
    if (isFull(stack)) {
        printf("Overflow\n");
        exit(EXIT_FAILURE);
    }
    stack->arr[++stack->top] = value;
}

char pop(stack_t* stack) {
    if (isEmpty(stack)) {
        return ' ';
    }

    char popped = stack->arr[stack->top];
    stack->top--;
    return popped;
}

static int cmp_scores(const void* s1, const void* s2) {
    unsigned long long score1 = (unsigned long long) *((unsigned long long* ) s1);
    unsigned long long score2 = (unsigned long long) *((unsigned long long* ) s2);
    if (score1 < score2) {
        return -1;
    } else if (score1 == score2) {
        return 0;
    } else {
        return 1;
    }
}

int score(char illegal_char) {
    switch (illegal_char) {
    case ')':
        return 3;
        break;
    case ']':
        return 57;
        break;
    case '}':
        return 1197;
        break;
    case '>':
        return 25137;
        break;
    default:
        printf("Invalid character: %c\n", illegal_char);
        exit(EXIT_FAILURE);
        break;
    }
}

int score_p2(char cl) {
    switch (cl) {
    case ')':
        return 1;
        break;
    case ']':
        return 2;
        break;
    case '}':
        return 3;
        break;
    case '>':
        return 4;
        break;
    default:
        printf("Invalid character: %c\n", cl);
        exit(EXIT_FAILURE);
        break;
    }
}

static int is_opening(char c) {
    return (c == '(' || c == '[' || c == '{' || c == '<');
}

static int matching(char op, char cl) {
    return ((op == '(' && cl == ')') ||
            (op == '[' && cl == ']') ||
            (op == '{' && cl == '}') ||
            (op == '<' && cl == '>'));
}

static char closing(char op) {
    switch (op) {
    case '(':
        return ')';
        break;
    case '[':
        return ']';
        break;
    case '{':
        return '}';
        break;
    case '<':
        return '>';
        break;
    default:
        printf("Invalid character: %c\n", op);
        exit(EXIT_FAILURE);
        break;
    }

}

int main() {
    FILE *file = fopen(INPUT, "r");

    char* line = NULL;

    unsigned int part1 = 0;
    unsigned long long part2 = 0;
    unsigned long long* part2_scores = malloc(100 * sizeof(unsigned long long));
    int nb_scores = 0;

    size_t len;
    while (getline(&line, &len, file) != -1) {
        // strip trailing newline from the nend
        line[strcspn(line, "\n")] = 0;

        stack_t queue;
        initialize(&queue);

        int corrupted = 0;
        for (size_t i = 0; i < strlen(line); i++) {
            char c = line[i];


            if (is_opening(c)) {
                push(&queue, c);
            } else {
                char last = pop(&queue);
                if (!matching(last, c)) {
                    corrupted = 1;
                    part1 += score(c);
                    break;
                }
            }
        }

        // a line that is not corrupted is not complete (as per the input, all lines are either
        // incomplete or corrupted)
        // to reconstruct missing closing chars, we just unpop the queue until it's empty
        if (!corrupted) {
            unsigned long long score = 0;
            while (!isEmpty(&queue)) {
                char c = pop(&queue);
                score *= 5;
                score += score_p2(closing(c));
            }
            part2_scores[nb_scores++] = score;
        }
    }

    // sort to get the median element
    qsort(part2_scores, nb_scores, sizeof(unsigned long long), cmp_scores);
    part2 = part2_scores[nb_scores / 2];
    printf("%d %lld\n", part1, part2);

    if (line) {
        free(line);
    }
    free(part2_scores);

    fclose(file);
    return 0;
}
