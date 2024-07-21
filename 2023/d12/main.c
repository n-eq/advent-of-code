// For some reason, part 2 takes quite some time even with memoization (around 20 minutes with -O3).
// So this code has certainly room for improvements.
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

#define MEMO_MAX 1000000

#ifdef PART2
#define RECORDS_MAX 100
#define GROUPS_MAX 100
#else
#define RECORDS_MAX 50
#define GROUPS_MAX 100
#endif

typedef enum spring_type_e {
    Operational = '.',
    Damaged = '#',
    Unknown = '?',
} spring_type_t;

#ifdef PART2
typedef struct memoize_data_s {
    int idx;
    int group_idx;
    int seen_in_group;
    long long nb_arr;
} memoize_data_t;

static memoize_data_t memoized_records[MEMO_MAX];
static int memoized_idx = 0;

static bool is_memoized(int idx, int group_idx, int seen_in_group, long long* res) {
    if (memoized_idx == 0) {
        return false;
    }

    for (int i = 0; i < MEMO_MAX; i++) {
        memoize_data_t m = memoized_records[i];
        if (m.idx == idx && m.group_idx == group_idx && m.seen_in_group == seen_in_group) {
            *res = m.nb_arr;
            return true;
        }
    }
    return false;
}

static void memoize(int idx, int group_idx, int seen_in_group, long long arr) {
    memoized_records[memoized_idx].idx = idx;
    memoized_records[memoized_idx].group_idx = group_idx;
    memoized_records[memoized_idx].seen_in_group = seen_in_group;
    memoized_records[memoized_idx].nb_arr = arr;

    memoized_idx++;
    if (memoized_idx == MEMO_MAX) {
        // simple eviction policy: take out the "oldest" element
        memoized_idx = 0;
    }
}

static void clear_memoize() {
    memoized_idx = 0;
    memset(memoized_records, 0, sizeof(memoize_data_t) * MEMO_MAX);
}
#endif

static long long find_arrangements(char* p, int* groups, int nb_groups, int idx, int group_idx, int seen_in_group) {
    long long res = 0;

#ifdef PART2
    if (is_memoized(idx, group_idx, seen_in_group, &res)) {
        return res;
    }
#endif

    if (idx == strlen(p)) {
        // reached the last char, either we have no more groups, or still have a group to 'close',
        // and the current count of 'Damaged' springs corresponds to the number required
        if (group_idx == nb_groups) {
            return seen_in_group == 0;
        }
        else if (group_idx  == nb_groups - 1) {
            return groups[group_idx] == seen_in_group;
        } else {
            return 0; // something's wrong
        }
    }

    spring_type_t ops[] = { Operational, Damaged };
    char* q = strdup(p);
    for (int i = 0; i < 2; ++i) {
        if (p[idx] == Unknown) 
            q[idx] = ops[i];

        if (p[idx] == ops[i] || p[idx] == Unknown) {
            switch (ops[i]) {
                case Operational:
                    if (seen_in_group == 0) {
                        // we're not currently counting '#'s, so move to the next character, and keep the data related to the group as is
                        res += find_arrangements(q, groups, nb_groups, idx + 1, group_idx, 0);
                    } else if (seen_in_group > 0 && group_idx < nb_groups && groups[group_idx] == seen_in_group) {
                        // we are currently counting '#', since we found a non '#', if the numbers (count and expected) match, then we 
                        // move to the next character and next group
                        res += find_arrangements(q, groups, nb_groups, idx + 1, group_idx + 1, 0);
                    }
                    break;
                case Damaged:
                    // new '#', increment seen_in_group and move on to the next character of the string, while keeping the same group
                    res += find_arrangements(q, groups, nb_groups, idx + 1, group_idx, seen_in_group + 1);
                    break;
                default:
                    break;
            }

        }
    }

    free(q);

#ifdef PART2
    memoize(idx, group_idx, seen_in_group, res);
#endif

    return res;
}

int main() {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char line[100];
    long long res = 0;
    int record_idx = 0;

    int* groups = malloc(GROUPS_MAX * sizeof(int));
    while (fgets(line, sizeof(line), f)) {
        clear_memoize();

        char* p = line;

        char* strp = p;
        while (*p != ' ') {
            p++;
        }
        strp[p - strp] = '\0';

#ifdef PART2
        char* unfold_records = malloc(strlen(strp) * 5 + 1 + 4 /* inner joining '?' */);
        for (int i = 0; i < 5; i++) {
            strcat(unfold_records, strp);
            if (i < 4)
                strcat(unfold_records, "?");
        }
        unfold_records[strlen(strp) * 5 + 4] = '\0';
        strp = unfold_records;
#endif

        groups = memset(groups, 0, GROUPS_MAX * sizeof(int));
        int nb_groups = 0;

        char* damaged_group = p + 1;
        groups[nb_groups++] = atoi(damaged_group);

        char* q;
        while ((q = strchr(damaged_group, ','))) {
            groups[nb_groups++] = atoi(++q);
            damaged_group = q;
        }


        struct timeval t1, t2;
        gettimeofday(&t1, NULL);

        long long tmp_res = 0;
#ifdef PART2
        int* unfold_groups = malloc(nb_groups * 5 * sizeof(int));
        if (!unfold_groups) {
            perror("failed to malloc unfold");
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < nb_groups; j++) {
                unfold_groups[i * nb_groups + j] = groups[j];
            }
        }

        nb_groups *= 5;

        tmp_res = find_arrangements(strp, unfold_groups, nb_groups, 0, 0, 0);
#else
        tmp_res = find_arrangements(strp, groups, nb_groups, 0, 0, 0);
#endif
        res += tmp_res;
        gettimeofday(&t2, NULL);
        // printf("line %d (%s) arrangements: %lld\n", record_idx, strp, tmp_res);

        record_idx++;

#ifdef PART2
        free(unfold_records);
        free(unfold_groups);
#endif
    }

    printf("res: %lld\n", res);

    free(groups);
    fclose(f);
}
