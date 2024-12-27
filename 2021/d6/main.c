#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_input(char* input, unsigned long int* fish_count) {
    for (int i = 0; i < 9; i++) {
        fish_count[i] = 0;
    }
    char *p;
    p = strtok (input, ",");
    while (p) {
        int ttl = atoi(p);
        fish_count[ttl]++;
        p = strtok (NULL, ",");
    }
}

unsigned long int count_fish(unsigned long int* fish_count) {
    unsigned long int res = 0;
    for (int i = 0; i < 9; i++) {
        res += fish_count[i];
    }
    return res;
}

void display_fish(unsigned long int fish_count[9]) {
    for (int i = 0; i < 9; i++) {
        for (unsigned long int j = 0; j < fish_count[i]; j++) {
            printf("%d,", i);
        }
    }
    printf("\n");
}

void display_fish_count(unsigned long int fish_count[9]) {
    printf("fish count : ");
    for (int i = 0; i < 9; i++) {
        printf("%ld ", fish_count[i]);
    }
    printf("\n");
}

void simulate(unsigned long int fish_count[9], int nb_days, int* part1, unsigned long int* part2) {
    for (int i = 0; i < nb_days; i++) {
#if 0
        printf("DAY %d ... ", i);
        display_fish(fish_count);
#endif

        if (i == 80) {
            *part1 = (int) count_fish(fish_count);
        }

        // new fish count is the number of fish with 0 TTL
        unsigned long int created_fish = fish_count[0];
        
        // Each fish with a non-null TTL decreases its TTL each day
        for (int f = 1; f < 9; f++) {
            fish_count[f - 1] = fish_count[f];
        }

        // fish with 0 TTL spawn fish with a TTL of 8
        fish_count[8] = created_fish;

        // fish that spawned new fish a respawned with a TTL of 6
        fish_count[6] += created_fish;
    }

    *part2 = count_fish(fish_count);
}

int main() {
    /* FILE *file = fopen("input_test", "r"); */
    FILE *file = fopen("input", "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return 1;
    }

    long length = 0;
    fseek(file, 0, SEEK_END);
    length = ftell (file);
    fseek(file, 0, SEEK_SET);
    char* line = (char* ) malloc(length * sizeof(char));
    if (!fread(line, 1, length, file)) {
        printf("Failed to read input file");
        exit(1);
    }

    // number of fish with each possible TTL (0, 1, ... 8)
    unsigned long int fish_count[9];
    parse_input(line, (unsigned long int* ) fish_count);

    int part1 = 0;
    unsigned long int part2 = 0;
    simulate(fish_count, 256, &part1, &part2);
    printf("part1: %d, part2: %ld\n", part1, part2);

    free(line);
    fclose(file);
    return 0;
}
