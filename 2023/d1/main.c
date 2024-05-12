#include <ctype.h>
#include <stdio.h>
#include <string.h>
    
int is_digit_extended(char* p, int* value) {
#ifdef PART1
    return isdigit(*p);
#endif

    if (isdigit(*p)) {
        *value = *p - '0';
        return 1;
    }

    if (strncmp(p, "one", 3) == 0) {
        *value = 1;
        return 1;
    } else if (strncmp(p, "two", 3) == 0) {
        *value = 2;
        return 1;
    } else if (strncmp(p, "three", 5) == 0) {
        *value = 3;
        return 1;
    } else if (strncmp(p, "four", 4) == 0) {
        *value = 4;
        return 1;
    } else if (strncmp(p, "five", 4) == 0) {
        *value = 5;
        return 1;
    } else if (strncmp(p, "six", 3) == 0) {
        *value = 6;
        return 1;
    } else if (strncmp(p, "seven", 5) == 0) {
        *value = 7;
        return 1;
    } else if (strncmp(p, "eight", 5) == 0) {
        *value = 8;
        return 1;
    } else if (strncmp(p, "nine", 4) == 0) {
        *value = 9;
        return 1;
    } 
    return 0;
}


int main() {
    char line[100];

    FILE *file = fopen("input", "r");

    int sum = 0;
    int first_digit = -1;
    int last_digit = 0;
    while (fgets(line, sizeof(line), file)) {
        int i = 0;
        while (line[i]) {
            int digit = 0;
            if (is_digit_extended(line + i, &digit)) {
                if (first_digit == -1) {
                    first_digit = digit;
                }
                last_digit = digit;
            }
            i++;
        }
        sum += first_digit * 10 + last_digit;
        first_digit = -1;
    }

    printf("result = %d\n", sum);

    fclose(file);
}
