#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

#define DISPLAY_VALS(vals, nb_lines, val_count) \
    for (int j = 0; j < nb_lines; j++) {\
        for (int _ = 0; _ < j; ++_) {\
            printf(" ");\
        }\
        for (int k = 0; k < val_count; k++) {\
            printf("%-2d ", vals[j][k]);\
        }\
        printf("\n");\
    }

static int get_next(int* vals, int count) {
    int nb_vecs = 1;
    int** vecs = malloc(sizeof(int* ) * nb_vecs);

#ifdef PART2
    count += 1; // one more (at the start of the vector) for part2
#endif

    vecs[0] = malloc(sizeof(int) * (1 + count));

#ifdef PART2
    memcpy((vecs[0] + 1), vals, sizeof(int) * count);
#else
    memcpy(vecs[0], vals, sizeof(int) * count);
#endif

    while (1) {
        vecs = realloc(vecs, sizeof(int*) * (nb_vecs + 1));

        int nb_elements = 1 + count - nb_vecs; // all elements + extrapolated value
        vecs[nb_vecs] = malloc(sizeof(int) * nb_elements);
        memset(vecs[nb_vecs], 0, sizeof(int) * nb_elements);

        int i;
        int start = 0;
#ifdef PART2
        start = 1; // For part2 the first element is reserved, so we start off by 1 index
#endif
        for (i = start; i < nb_elements - 1; ++i) {
            vecs[nb_vecs][i] = vecs[nb_vecs - 1][i + 1] - vecs[nb_vecs - 1][i];
        }

        bool allzero = true;
        for (i = 0; i < nb_elements - 1; ++i) {
            if (vecs[nb_vecs][i] != 0) {
                allzero = false;
                break;
            }
        }

        if (allzero) {
            break;
        }

        nb_vecs++;
    }

    // extrapolate bottom up
    for (int i = nb_vecs - 1; i >= 0; --i) {
#ifdef PART2
        vecs[i][0] = vecs[i][1] - vecs[i + 1][0];
#else
        int nb_elements = count + 1 - i;

        int val1 = vecs[i][nb_elements - 2];
        int val2 = vecs[i + 1][nb_elements - 2];
        int new_val = val1 + val2;

        vecs[i][nb_elements - 1] = new_val;
#endif
    }

#ifdef PART2
    int res = vecs[0][0];
#else
    int res = vecs[0][count];
#endif

    for (int i = 0; i < nb_vecs + 1; i++) {
        free(vecs[i]);
    }
    free(vecs);

    return res;
}

int main() {
    char line [1000];

    FILE *file = fopen(INPUT, "r");
    if (file == NULL) {
        printf("Error: can't open file.\n");
        return 1;
    }

    int nb_lines = 0;
    int val_count = 0;
    
    int** vals = malloc(sizeof(int*));
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') {
            continue;
        }

       vals = realloc(vals, sizeof(int*) * (nb_lines + 1));
       char* p = line;

       if (!val_count) {
           char* q = line;
           int j;
           for (j = 0; q[j]; ++j) {
               if (q[j] == ' ') {
                   val_count++;
               }
           }
           val_count++;
       }

       vals[nb_lines] = malloc(sizeof(int) * val_count);

       int j = 0;
       p = strtok (p, " ");
       while (p != NULL) {
           vals[nb_lines][j] = atoi(p);
           p = strtok(NULL, " ");
           j++;
       }
       nb_lines++;
    }

    // DISPLAY_VALS

    int sum = 0;
    for (int i = 0; i < nb_lines; i++) {
        sum += get_next(vals[i], val_count);
    }
    printf("sum = %d\n", sum);

    for (int i = 0; i < nb_lines; i++) {
        free(vals[i]);
    }
    free(vals);

}
