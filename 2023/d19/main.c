// This one looks quite similar to 2023 day 5 where seeds became ranges of seeds in part2.
// However, for this day it was quite impossible/impractical to use threads, unlike day 5.
// Instead, to deal with ranges, we just split ranges depending on the conditions where the
// intervals overlap.
// There is too much parsing in this one, and the code is quite refactorable, but it works.
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef TEST
#define NB_WORKFLOWS 20
#else
#define NB_WORKFLOWS 600
#endif

#ifdef PART2
#define NB_PARTS_MAX 10000
#endif

#define NB_ELEMENTS(v) (long) (((long) (v[1])) - ((long) (v[0])) + 1)
#define INPUT argv[1]

typedef enum verdict_e {
    Accepted = 'A',
    Refused = 'R',
    Unknown = 'U',
} verdict_t;

typedef enum comparison_e {
    // Equal = '=',
    Greater = '>',
    Less = '<',
} comparison_t;

// As a last resort, a workflow can either be a verdict or the name of another workflow.
typedef struct fallback_s {
    union {
        verdict_t verdict;
        char* workflow;
    } u;
    bool is_verdict;
} fallback_t;

typedef enum category_e {
    X = 'x',
    M = 'm',
    A = 'a',
    S = 's',
} category_t;

typedef struct condition_s {
    category_t category;
    comparison_t comparison;
    int val;
    const char* output; // next workflow if the condition is met
} condition_t;

typedef struct workflow_s {
    char* name;
    condition_t* conditions;
    int nb_conditions;
    fallback_t fallback;
} workflow_t;

#ifndef PART2
typedef struct part_s {
    int x;
    int m;
    int a;
    int s;
} part_t;
#else
// For part 2, we consider each part as an inclusive range for each of x, m, a, s
typedef struct part_s  {
    int x[2];
    int m[2];
    int a[2];
    int s[2];
    workflow_t state; // we need to keep track of the current state because of splits
    int condition_idx;
} part_t;
#endif

workflow_t* workflows;
static int nb_wokflows;
static long res;

void free_workflows() {
    for (int i = 0; i < nb_wokflows; i++) {
        free(workflows[i].name);
        for (int j = 0; j < workflows[i].nb_conditions; j++) {
            free((void*) workflows[i].conditions[j].output);
        }
        if (!workflows[i].fallback.is_verdict) {
            free(workflows[i].fallback.u.workflow);
        }
        free(workflows[i].conditions);
    }
    free(workflows);

}

workflow_t* locate_workflow(const char* name) {
    for (int i = 0; i < nb_wokflows; i++) {
        if (strcmp(workflows[i].name, name) == 0) {
            return &workflows[i];
        }
    }
    return NULL;
}

#ifndef PART2
void parse_part(char* s, part_t* p) {
    s = strchr(s, '{') + 1;
    char* s2 = strchr(s, '}');
    *s2 = '\0';

    s = strtok(s, ",");
    while (s) {
        char* s2 = strchr(s, '=');
        *s2 = '\0';
        if (*s == 'x') {
            p->x = atoi(s2 + 1);
        } else if (*s == 'm') {
            p->m = atoi(s2 + 1);
        } else if (*s == 'a') {
            p->a = atoi(s2 + 1);
        } else if (*s == 's') {
            p->s = atoi(s2 + 1);
        }
        s = strtok(NULL, ",");
    }
}
#endif

void parse_workflow(char* p, workflow_t* workflow) {
    char* p2 = strdup(p);
    char* q = strchr(p2, '{');
    *q = '\0';
    workflow->name = strdup(p2);
    workflow->conditions = malloc(10 * sizeof(condition_t)); // generous enough, we'll allocate 10 by default
    workflow->nb_conditions = 0;

    p = strchr(p, '{') + 1;
    p = strtok(p, ",");
    while (p) {
        if (strchr(p, ':')) {
            char* p2 = strdup(p);
            char* q = strchr(p2, ':');
            *q = '\0';
            workflow->conditions[workflow->nb_conditions].category = *p2;
            workflow->conditions[workflow->nb_conditions].comparison = *(p2 + 1);
            workflow->conditions[workflow->nb_conditions].val = atoi(p2 + 2);
            p = strchr(p, ':') + 1;
            workflow->conditions[workflow->nb_conditions].output = strdup(p);
            workflow->nb_conditions++;
        } else {
            if (*p == 'A' || *p == 'R') {
                workflow->fallback.is_verdict = true;
                workflow->fallback.u.verdict = *p;
            } else {
                workflow->fallback.is_verdict = false;
                char* q = strchr(p, '}');
                *q = '\0';
                workflow->fallback.u.workflow = strdup(p);
            }
        }
        p = strtok(NULL, ",");
    }

}

#ifndef PART2
bool condition_is_met(condition_t c, part_t* p) {
    int val = 0;
    if (c.category == 'x') {
        val = p->x;
    } else if (c.category == 'm') {
        val = p->m;
    } else if (c.category == 'a') {
        val = p->a;
    } else if (c.category == 's') {
        val = p->s;
    }

    if (c.comparison == '>') {
        return val > c.val;
    } else if (c.comparison == '<') {
        return val < c.val;
    }

    return false;
}

verdict_t get_verdict(part_t* part) {
    workflow_t* w = locate_workflow("in");
    assert(w);

    verdict_t verdict = Unknown;
    while (verdict == Unknown) {
        bool found = false;
        for (int j = 0; j < w->nb_conditions; j++) {
            condition_t* condition = &w->conditions[j];
            if (condition_is_met(*condition, part)) {
                if (*(condition->output) == 'A' || *(condition->output) == 'R') {
                    verdict = *(condition->output);
                } else {
                    w = locate_workflow(condition->output);
                }
                found = true;
                break;
            }
        }

        if (!found) {
            if (w->fallback.is_verdict) {
                verdict = w->fallback.u.verdict;
            } else {
                w = locate_workflow(w->fallback.u.workflow);
            }
        }
    }

    return verdict;
}
#else
// Process a given part, this will either produce a verdict (we return the result of acceptance)
// or populate two children
long process(part_t* part, part_t* children, int* nb_children) {
    workflow_t w = part->state;

    long raw_res = NB_ELEMENTS(part->x) * NB_ELEMENTS(part->m) * NB_ELEMENTS(part->a) * NB_ELEMENTS(part->s);

    if (part->condition_idx == w.nb_conditions) {
        if (w.fallback.is_verdict) {
            if (w.fallback.u.verdict == Accepted) {
                // we return the volume of the part
                return raw_res;
            } else {
                return 0;
            }
        } else {
            // We start off with a new children, same ranges but different workflow
            memcpy(children, part, sizeof(part_t));
            children->state = *(locate_workflow(w.fallback.u.workflow));
            children->condition_idx = 0;
            *nb_children = 1;
        }
        return 0;
    }

    condition_t c = w.conditions[part->condition_idx];
    int* val;
    int* x = part->x;
    int* m = part->m;
    int* a = part->a;
    int* s = part->s;
    raw_res = 0;
    if (c.category == 'x') {
        val = x;
        raw_res = NB_ELEMENTS(m) * NB_ELEMENTS(a) * NB_ELEMENTS(s);
    } else if (c.category == 'm') {
        val = m;
        raw_res = NB_ELEMENTS(x) * NB_ELEMENTS(a) * NB_ELEMENTS(s);
    } else if (c.category == 'a') {
        val = a;
        raw_res = NB_ELEMENTS(x) * NB_ELEMENTS(m) * NB_ELEMENTS(s);
    } else if (c.category == 's') {
        val = s;
        raw_res = NB_ELEMENTS(x) * NB_ELEMENTS(m) * NB_ELEMENTS(a);
    }

    raw_res *= (long) (val[1] - val[0] + 1);

    if (c.comparison == '>') {
        if (val[0] > c.val) { // range is fully inside the interval (condition satisfied)
            if (*(c.output) == 'A') {
                return raw_res;
            } else if (*(c.output) != 'R') {
                // to avoid reusing part variable (since it's popped), we'll populate the first children
                // and update nb_children
                memcpy(children, part, sizeof(part_t));
                part->state = *(locate_workflow(c.output));
                part->condition_idx = 0;
                *nb_children = 1;
            }
        } else {
            memcpy(children, part, sizeof(part_t));
            memcpy(children + 1, part, sizeof(part_t));
            *nb_children = 2;

            part_t* child_out = children; // doesn't satisfy the condition
            part_t* child_in = children + 1; // satisfies the condition

            if (c.category == 'x') {
                child_in->x[0] = c.val + 1;
                child_out->x[1] = c.val;
            } else if (c.category == 'm') {
                child_in->m[0] = c.val + 1;
                child_out->m[1] = c.val;
            } else if (c.category == 'a') {
                child_in->a[0] = c.val + 1;
                child_out->a[1] = c.val;
            } else if (c.category == 's') {
                child_in->s[0] = c.val + 1;
                child_out->s[1] = c.val;
            }

            child_out->condition_idx++;

            if (*(c.output) == 'A') {
                *nb_children = 1;
                return NB_ELEMENTS(child_in->x) * NB_ELEMENTS(child_in->m) * NB_ELEMENTS(child_in->a) * NB_ELEMENTS(child_in->s);
            } else if (*(c.output) != 'R') {
                child_in->state = *(locate_workflow(c.output));
                child_in->condition_idx = 0;
            } else {
                *nb_children = 1;
            }
        }
    } else if (c.comparison == '<') {
        if (val[1] < c.val) { // range is fully inside the interval (condition satisfied)
            if (*(c.output) == 'A') {
                return raw_res;
            } else if (*(c.output) != 'R') {
                // to avoid reusing part variable (since it's popped), we'll populate the first children
                // and update nb_children
                memcpy(children, part, sizeof(part_t));
                part->state = *(locate_workflow(c.output));
                part->condition_idx = 0;
                *nb_children = 1;
            }
        } else {
            // we need to split
            // only the first children follows the condition
            // for the second children we update condition_idx
            memcpy(children, part, sizeof(part_t));
            memcpy(children + 1, part, sizeof(part_t));
            *nb_children = 2;

            part_t* child_out = children; // satisfies the condition
            part_t* child_in = children + 1; // satisfies the condition

            if (c.category == 'x') {
                child_in->x[1] = c.val - 1;
                child_out->x[0] = c.val;
            } else if (c.category == 'm') {
                child_in->m[1] = c.val - 1;
                child_out->m[0] = c.val;
            } else if (c.category == 'a') {
                child_in->a[1] = c.val - 1;
                child_out->a[0] = c.val;
            } else if (c.category == 's') {
                child_in->s[1] = c.val - 1;
                child_out->s[0] = c.val;
            }

            child_out->condition_idx++;

            if (*(c.output) == 'A') {
                *nb_children = 1;
                return NB_ELEMENTS(child_in->x) * NB_ELEMENTS(child_in->m) * NB_ELEMENTS(child_in->a) * NB_ELEMENTS(child_in->s);
            } else if (*(c.output) != 'R') {
                child_in->state = *(locate_workflow(c.output));
                child_in->condition_idx = 0;
            } else {
                *nb_children = 1;
            }
        }
    }

    return 0;
}
#endif


#ifdef PART2
void pop(part_t* parts, part_t* p, int* nb_parts) {
    memcpy(p, &parts[0], sizeof(part_t));
    memcpy(parts, parts + 1, (*nb_parts - 1) * sizeof(part_t));
    (*nb_parts)--;
}

void push(part_t* parts, int* nb_parts, part_t* p) {
    parts[*nb_parts] = *p;
    (*nb_parts)++;
}
#endif

int main(int argc, char** argv) {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    workflows = malloc(NB_WORKFLOWS * sizeof(workflow_t));

    char line[1000];
    bool workflow = true; // flipped when reaching an empty line
    while (fgets(line, sizeof(line), f)) {
        char* p = line;

        if (*p == '\n') {
            workflow = false;
            continue;
        }

        if (workflow) {
            parse_workflow(p, &workflows[nb_wokflows]);
            nb_wokflows++;
        } else {
#ifndef PART2
            part_t part;
            parse_part(p, &part);

            verdict_t v = get_verdict(&part);

            if (v == Accepted) {
                res += part.x + part.m + part.a + part.s;
            }
#else
            break; // For part 2 no parsing is needed
#endif
        }
    }

#ifdef PART2
    workflow_t* start = locate_workflow("in");
    part_t initial_part = {
        .x = { 1, 4000 },
        .m = { 1, 4000 },
        .a = { 1, 4000 },
        .s = { 1, 4000 },
        .state = *start,
        .condition_idx = 0
    };

    // The idea for Part 2 is to start from the global range [1: 4000] * 4,
    // and  either increment the result if we reached a positive verdict, or push
    // new parts (children) with new ranges pointing to new workflows
    part_t* parts = malloc(1000 * sizeof(part_t));
    int nb_parts = 0;
    push(parts, &nb_parts, &initial_part);

    part_t* children = malloc(2 * sizeof(part_t));
    while (nb_parts != 0) {
        part_t* p = malloc(sizeof(part_t));
        pop(parts, p, &nb_parts);

        memset(children, 0, 2 * sizeof(part_t));

        int nb_children = 0;
        res += process(p, children, &nb_children);

        for (int i = 0; i < nb_children; i++) {
            push(parts, &nb_parts, children + i);
        }

        free(p);
    }
    free(children);
#endif

    printf("res = %ld\n", res);

    free_workflows();
    fclose(f);
}
