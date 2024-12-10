#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "module.h"

// #define INPUT "input_test"
#define INPUT argv[1]

void dbg(module_t* m) {
    printf("%s (%s) -> ", m->name, m->type == FlipFlop ? "FlipFlop" : m->type == Conjunction ? "Conjunction" : "Broadcaster");
    for (int i = 0; i < m->nb_outputs; i++) {
        printf("%s, ", m->outputs[i]);
    }
    printf("\n");
}

int parse_input(FILE* f, module_t* modules) {
    char line[1000];
    int i = 0;
    while (fgets(line, sizeof(line), f)) {
        char* p = line;

        if (*p == '\n') {
            continue;
        }

        // remove trailing newline character
        p[strcspn(p, "\n")] = 0;

        char* left = strdup(p);
        char* delim = strchr(left, '-');
        char* right = strdup(delim + 3);

        *(delim - 1) = '\0'; // there is a space before the '-'
        char* name;
        module_type_t type;
        if (*left == '%' || *left == '&') {
            if (*left == '%') {
                type = FlipFlop;
            } else {
                type = Conjunction;
            }

            left++;
            name = strdup(left);
        } else {
            assert(strcmp(left, "broadcaster") == 0);
            name = strdup("broadcaster");
            type = Broadcaster;
        }
        module_t* m = find_module(modules, i, name);
        if (m) {
            continue;
        } else {
            m = &modules[i];
            m->name = strdup(name);
            m->type = type;
            m->state = Off;
            m->nb_outputs = 0;
            m->nb_mrp = 0;
        }

        i++;
    }

    int nb_modules = i;

    // read a second time
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        char* p = line;

        if (*p == '\n') {
            continue;
        }

        // remove trailing newline character
        p[strcspn(p, "\n")] = 0;

        char* left = strdup(p);
        char* delim = strchr(left, '-');
        char* right = strdup(delim + 3);

        *(delim - 1) = '\0'; // there is a space before the '-'
        char* name;
        module_type_t type;
        if (*left == '%' || *left == '&') {
            if (*left == '%') {
                type = FlipFlop;
            } else {
                type = Conjunction;
            }

            left++;
            name = strdup(left);
        } else {
            assert(strcmp(left, "broadcaster") == 0);
            name = strdup("broadcaster");
        }

        module_t* input = find_module(modules, i, name);
        if (!input) {
            perror("this shouldn't happen!");
            exit(2);
        }

        input->outputs = (char** ) malloc(0 * sizeof(char*));

        char* output_name = strtok(right, ", ");
        while (output_name) {
            input->outputs = (char**) reallocarray(input->outputs, input->nb_outputs + 1, sizeof(char*));
            input->outputs[input->nb_outputs] = strdup(output_name);
            input->nb_outputs++;

            module_t* output = find_module(modules, nb_modules, output_name);
            if (!output) {
                goto next_token; // virtual module (not parsed because it's doesn't have any output)
            }

            if (output->type == Conjunction) {
                output->mrps = (mrp_t* ) reallocarray(output->mrps, output->nb_mrp + 1, sizeof(mrp_t));
                output->mrps[output->nb_mrp] = (mrp_t) {
                    .input = strdup(input->name),
                    .pulse = Low,
                };
                output->nb_mrp++;
            }

next_token:
            output_name = strtok(NULL, ", ");
        }

        i += 1;
    }

    return nb_modules;
}

static bool cycle(module_t* modules, int nb_modules, int push, const int nb_pushes) {
    bool all_flip_flops_off = true;
    bool all_conjunction_modules_mrp_high = true;

    for (int i = 0; i < nb_modules; i++) {
        if (modules[i].type == FlipFlop) {
            if (modules[i].state == On) {
                all_flip_flops_off = false;
                break;
            }
        } else if (modules[i].type == Conjunction) {
            module_t* m = &modules[i];
            for (int j = 0; j < m->nb_mrp; j++) {
                if (m->mrps[j].pulse == Low) {
                    all_conjunction_modules_mrp_high = false;
                    break;
                }
            }
        }
        if (!all_conjunction_modules_mrp_high || !all_flip_flops_off) {
            // double break to shortcut the outer loop
            break;
        }
    }

    return push > 0 && push != (nb_pushes - 1) && all_flip_flops_off && all_conjunction_modules_mrp_high;
}

void process(module_t* modules, int nb_modules, const int nb_pushes, int* low_pulses, int* high_pulses) {
    for (int push = 0; push < nb_pushes; push++) {
        event_queue_t queue;
        init_queue(&queue, 1000);

        // initial pulse
        push_back(&queue, (event_t) {
            .pulse = Low,
            .dest = "broadcaster",
            .src = "broadcaster", // don't really care for this one
        });

        int low = 0;
        int high = 0;

        int i = 0;
        event_t* event;
        while ((event = pop(&queue))) {
            if (cycle(modules, nb_modules, push, nb_pushes)) {
                printf("reached a cycle at %d (%d, %d)\n" , push, *low_pulses, *high_pulses);

                int factor, remain;
                factor = nb_pushes / push;
                remain = nb_pushes % push;

                (*low_pulses) *= factor;
                (*high_pulses) *= factor;

                if (remain != 0) {
                    push *= factor; // there are still a few pushes to process
                    printf("next push index %d\n", push);
                } else {
                    printf("perfect division\n");
                    goto done;
                }
            }

            // printf("%s(%s)\n", event->pulse == High ? "HIGH" : "LOW", event->dest);

            if (event->pulse == Low) { 
                low++;
            } else {
                high++;
            }

            module_t* m = find_module(modules, nb_modules, event->dest);
            if (!m) {
                // virtual module (doesn't have any output so not parsed from the input)
                i++;
                continue;
            }

            pulse_type_t conjunction_pulse;
            switch (m->type) {
            case Broadcaster:
                // send the same pulse to all output modules
                for (int j = 0; j < m->nb_outputs; j++) {
                    event_t new_event = (event_t) {
                        .pulse = event->pulse,
                        .src = strdup(m->name),
                        .dest = strdup(m->outputs[j]),
                    };
                    push_back(&queue, new_event);
                }
                break;
            case FlipFlop:
                if (event->pulse == Low) {
                    for (int j = 0; j < m->nb_outputs; j++) {
                        event_t new_event = (event_t) {
                            .pulse = (m->state == Off)? High : Low,
                            .src = strdup(m->name),
                            .dest = strdup(m->outputs[j]),
                        };
                        push_back(&queue, new_event);
                    }
                    m->state = (state_t) (m->state + 1) % 2;
                }
                break;
            case Conjunction:
                // First we need to update MRP values with the pulse we just received
                for (int i = 0; i < m->nb_mrp; i++) {
                    mrp_t* mrp = &m->mrps[i];
                    if (strcmp(find_module(modules, nb_modules, event->src)->name, mrp->input) == 0) {
                        // update MRP
                        mrp->pulse = event->pulse;
                        break;
                    }
                }

                // ... then check the MRPs
                bool all_high = true;
                for (int i = 0; i < m->nb_mrp; i++) {
                    mrp_t* mrp = &m->mrps[i];
                    if (mrp->pulse == Low) {
                        all_high = false;
                        break;
                    }
                }

                if (all_high) {
                    conjunction_pulse = Low;
                } else {
                    conjunction_pulse = High;
                }

                for (int j = 0; j < m->nb_outputs; j++) {
                    event_t new_event = (event_t) {
                        .pulse = conjunction_pulse,
                        .src = strdup(m->name),
                        .dest = strdup(m->outputs[j]),
                    };
                    push_back(&queue, new_event);
                }

                break;
            }

            i++;
        }

        *low_pulses += low;
        *high_pulses += high;

        free(queue.events);
    }

done:
    ;
}

int main(int argc, char** argv) {
    FILE* f = fopen(INPUT, "r");
    if (!f) {
        perror("fopen failed");
        return 1;
    }

    module_t* modules = malloc(100 * sizeof(module_t));
    int nb_modules = parse_input(f, modules);

    int nb_pushes = 1000;

    int low_pulses = 0;
    int high_pulses = 0;
    process(modules, nb_modules, nb_pushes, &low_pulses, &high_pulses);

    printf("part1: %d\n", low_pulses * high_pulses);

    free_modules(modules, nb_modules);
    fclose(f);
}
