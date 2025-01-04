#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

typedef char component[3];

typedef struct link_s {
    component comp1;
    component comp2;
} link_t;

link_t* links;
int nb_links = 0;

static inline int link_is_components(link_t* link, component c1, component c2) {
    if ((memcmp(link->comp1, c1, sizeof(component)) == 0 && memcmp(link->comp2, c2, sizeof(component)) == 0) ||
        (memcmp(link->comp1, c2, sizeof(component)) == 0 && memcmp(link->comp2, c1, sizeof(component)) == 0)) {
        return 1;
    }

    return 0;
}

int main() {
    FILE* f = fopen(INPUT, "r");

    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        char* p = strdup(line);

        component src;
        src[0] = *p++;
        src[1] = *p++;
        src[2] = *p;

        p = strchr(p, ':')  + 1;
        p = strtok(p, " ");
        while (p) {
            component target;
            target[0] = *p++;
            target[1] = *p++;
            target[2] = *p;

            links = realloc(links, (1 + nb_links) * sizeof(link_t));
            memcpy(&links[nb_links].comp1[0], src, sizeof(component));
            memcpy(&links[nb_links].comp2[0], target, sizeof(component));
            nb_links++;

            p = strtok(NULL, " ");
        }

        free(p);
    }

    // draw the graph using graphviz and neato engine
    // neato -Tsvg graph.dot -o output.svg
    FILE* graph = fopen("graph.dot", "w");
    fprintf(graph, "graph G {\n");
    for (int i = 0; i < nb_links; i++) {
        fprintf(graph, "%c%c%c -- %c%c%c\n",
                links[i].comp1[0],
                links[i].comp1[1],
                links[i].comp1[2],
                links[i].comp2[0],
                links[i].comp2[1],
                links[i].comp2[2]);
    }
    fprintf(graph, "}\n");
    fclose(graph);

    // This helps visualize the 3 links that need to be cut
    // in order to have 2 distinct graphs
    // GROUP1 -- GROUP2
    // pbx    -- njx
    // zvk    -- sxx
    // sss    -- pzr

    // count the number of components (nodes) in the graph
    // this is necessary since there are two "cliques" found after removing the 3 edges
    // so we only need to count the nodes in one of the cliques to get the other value
    component* all_nodes = NULL;
    int node_count = 0;
    for (int i = 0; i < nb_links; i++) {
        component c1, c2;
        memcpy(&c1, &links[i].comp1, sizeof(component));
        memcpy(&c2, &links[i].comp2, sizeof(component));

        int c1seen = 0;
        int c2seen = 0;
        for (int j = 0; j < node_count; j++) {
            if (all_nodes[j][0] == c1[0] && all_nodes[j][1] == c1[1] && all_nodes[j][2] == c1[2]) {
                c1seen = 1;
            }
            if (all_nodes[j][0] == c2[0] && all_nodes[j][1] == c2[1] && all_nodes[j][2] == c2[2]) {
                c2seen = 1;
            }
            if (c1seen && c2seen) {
                break;
            }
        }

        if (!c1seen) {
            all_nodes = realloc(all_nodes, (1 + node_count) * sizeof(component));
            memcpy(all_nodes + node_count, &c1, sizeof(component));
            node_count++;
        }
        if (!c2seen) {
            all_nodes = realloc(all_nodes, (1 + node_count) * sizeof(component));
            memcpy(all_nodes + node_count, &c2, sizeof(component));
            node_count++;
        }
    }
    free(all_nodes);

    // Next, count the number of components in one of the graphs using BFS (start from pbx component)
#define QUEUE_SIZE 10024
    component visited[QUEUE_SIZE] = { 0 };
    int nb_visited = 0;

    component start = { 'p', 'b', 'x' };
    component queue[QUEUE_SIZE] = { 0 };
    memcpy(&queue[0],  &start, sizeof(component));
    int queue_size = 1;
    while (queue_size) {
        component current;
        memcpy(&current, queue, sizeof(component)); // pop_front

        if (--queue_size) {
            memmove(queue, queue + 1, queue_size * sizeof(component));
        }

        int already_visited = 0;
        for (int i = 0; i < nb_visited; i++) {
            if (memcmp(&visited[i], &current, sizeof(component)) == 0) {
                already_visited = 1;
                break;
            }
        }

        if (already_visited) {
            continue;
        }

        component next;
        for (int i = 0; i < nb_links; i++) {
            link_t* link = links + i;
            if (memcmp(link->comp1, current, sizeof(component)) == 0) {
                memcpy(&next, &link->comp2, sizeof(component));
            } else if (memcmp(link->comp2, current, sizeof(component)) == 0) {
                memcpy(&next, &link->comp1, sizeof(component));
            } else {
                continue;
            }

            // Skip blacklisted links
            if (link_is_components(link, "pbx", "njx") ||
                link_is_components(link, "zvk", "sxx") ||
                link_is_components(link, "sss", "pzr")) {
                continue;
            }

            int already_visited = 0;
            for (int j = 0; j < nb_visited; j++) {
                if (visited[j][0] == next[0] && visited[j][1] == next[1] && visited[j][2] == next[2]) {
                    already_visited = 1;
                    break;
                }
            }

            if (!already_visited) {
                memcpy(queue + queue_size, next, sizeof(component));
                queue_size++;
            }
        }

        memcpy(visited + nb_visited, &current, sizeof(component));
        nb_visited++;
    }

    fprintf(stdout, "result: %d\n", nb_visited * (node_count - nb_visited));


    free(links);

    fclose(f);
}
