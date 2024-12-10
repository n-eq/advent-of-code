#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "module.h"

#define INPUT argv[1]

long lcm(long a, long b) {
    long max = (a > b)? a : b;
    long lcm = max;

    while ((lcm % a != 0) || (lcm % b != 0)) {
        lcm += max;
    }
    return lcm;
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

void process(module_t* modules, int nb_modules, const int nb_pushes, int* low_pulses, int* high_pulses,
        char** feeder_flip_flops, int nb_feeders, char* conjunction, long* part2) {

    // I made some assumptions here in terms of input parameters
    // but it's late at night and I spend most of my day working on this
    // so readability shouldn't be expected
    assert(nb_feeders == 4);

    *part2 = 1;
    long push = 0;

    bool found[4] = { false, false, false, false };
    while (1) {
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
                printf("reached a cycle at %ld (%d, %d)\n" , push, *low_pulses, *high_pulses);

                int factor, remain;
                factor = nb_pushes / push;
                remain = nb_pushes % push;

                (*low_pulses) *= factor;
                (*high_pulses) *= factor;

                if (remain != 0) {
                    push *= factor; // there are still a few pushes to process
                    printf("next push index %ld\n", push);
                }
                // in part1 we would go to done in the else case, but we need to keep pushing until we
                // find all the min number of presses to reach the goal
            }

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

            // PART2, check the current pulse is one of the four we're looking for!
            // if they're all found, go to the end of the function (this only works because all
            // the numbers we're looking for are greater than 1000 pushes used in part 1)
            if (strcmp(event->dest, conjunction) == 0) {
                for (int f = 0; f < nb_feeders; f++) {
                    if (strcmp(feeder_flip_flops[f], event->src) == 0 && event->pulse == High) {
                        *part2 = lcm(*part2, push + 1);
                        found[f] = true;
                        if (found[0] && found[1] && found[2] && found[3]) {
                            goto done;
                        }
                        break; // there can only be one at a time ;)
                    }
                }
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

        if (push < nb_pushes) {
            *low_pulses += low;
            *high_pulses += high;
        }

        free(queue.events);
        push++;
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


    // for part2, you'll notice 'rx' can only be reached from a conjunction module
    // that has 4 inputs (flip-flops)
    // sending LOW to 'rx' means the conjunction module's MRPs were all LOW
    // which means we need to find the number of pushes required for each input to send a LOW event.
    // then, it's just a matter of LCM
    module_t* conjunction_feeder;
    for (int i = 0; i < nb_modules; i++) {
        module_t* m = modules + i;
        for (int j = 0; j < m->nb_outputs; j++) {
            if (strcmp(m->outputs[j], "rx") == 0) {
                conjunction_feeder = m;
                break;
            }
        }
    }
    assert(conjunction_feeder->type == Conjunction);

    char** feeders = malloc(4 * sizeof(char*));
    int nb_feeders = 0;
    for (int i = 0; i < nb_modules; i++) {
        module_t* m = modules + i;
        for (int j = 0; j < m->nb_outputs; j++) {
            if (strcmp(m->outputs[j], conjunction_feeder->name) == 0) {
                feeders[nb_feeders++] = strdup(m->name);
            }
        }
    }
    assert(nb_feeders == 4);


    int low_pulses = 0;
    int high_pulses = 0;

    long part2 = 0;
    process(modules, nb_modules, nb_pushes, &low_pulses, &high_pulses,
            feeders, nb_feeders, conjunction_feeder->name, &part2);

    printf("part1: %d, part2: %ld \n", low_pulses * high_pulses, part2);

    free_modules(modules, nb_modules);
    fclose(f);
}
