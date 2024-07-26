#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    long res = 0;

    char line[1000];
    int lines = 0;
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        printf("%s", p);

        lines++;
    }

    printf("res: %ld\n", res);

    fclose(f);
}
