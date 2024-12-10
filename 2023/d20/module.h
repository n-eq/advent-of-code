#ifndef __MODULE_H__
#define __MODULE_H__ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum module_type_e {
   FlipFlop,
   Conjunction,
   Broadcaster,
} module_type_t;

typedef enum pulse_type_e {
    Low,
    High,
} pulse_type_t;

typedef struct mrp_s {
    pulse_type_t pulse;
    char* input;
} mrp_t;

typedef enum state_e {
    Off = 0,
    On = 1,
} state_t;

typedef struct module_s {
    char* name;
    module_type_t type;

    state_t state;

    mrp_t* mrps;
    int nb_mrp;

    char** outputs;
    size_t nb_outputs;
} module_t;

typedef struct event_s {
    pulse_type_t pulse;
    char* src;
    char* dest;
} event_t;

// a ring buffer to handle events
// at some points I thought the events were grewing indefinitely 
// and a static allocation wasn't enough, so I ended up implementing
// this structure to queue events
typedef struct event_queue_s {
    event_t* events;
    size_t tail;
    size_t head;
    size_t size;
    bool full;
} event_queue_t;


void free_queue(event_queue_t* q) {
    if (q) {
        if (q->events) {
            free(q->events);
        }
        free(q);
    }
}

// q is already allocated by the caller
void init_queue(event_queue_t* q, size_t size) {
    q->size = size;
    q->full = false;
    q->tail = q->head = 0;
    q->events = (event_t* ) malloc(size * sizeof(event_t));
}

bool is_full(event_queue_t* q) {
    if (q) {
        return q->full;
    } else {
        printf("q is NULL!!\n");
        return true;
    }
}

event_t* pop(event_queue_t* queue) {
    if (!queue) {
        return NULL;
    }

    if (!is_full(queue) && queue->tail == queue->head) {
        return NULL;
    }

    event_t* result = (event_t* ) malloc(sizeof(event_t));
    memcpy(result, &queue->events[queue->head], sizeof(event_t));

    queue->head = (queue->head + 1) % queue->size;
    queue->full = false;

    return result;
}

void push_back(event_queue_t* queue, event_t event) {
    if (is_full(queue)) {
        // this is no more needed actually
        size_t new_size = 1.1f * queue->size;
        queue->events = (event_t* ) realloc(queue->events, new_size * sizeof(event_t));
        queue->size = new_size;
        queue->full = false;
        printf("QUEUE EXTENDED %ld\n", new_size);
    }

    memcpy(&queue->events[queue->tail], &event, sizeof(event_t));
    queue->tail = (queue->tail + 1) % queue->size;

    if (queue->tail == queue->head) {
        queue->full = true;
    }
}

module_t* find_module(module_t* modules, int len, char* m) {
    module_t* mod = NULL;
    for (int i = 0; i < len; ++i) {
        if (strcmp(modules[i].name, m) == 0) {
            mod = &modules[i];
            break;
        }
    }
    return mod;
}

void free_modules(module_t* modules, int nb_modules) {
    for (int i = 0; i < nb_modules; i++) {
        module_t* m = modules + i;
        free(m->name); // they all have a name

        if (m->mrps) {
            free(m->mrps);
        }

        for (int j = 0; j < m->nb_outputs; j++) {
            free(m->outputs[j]);
        }
        if (m->outputs) {
            free(m->outputs);
        }
    }
    free(modules);
}

#endif
