#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static long gcd(long a, long b) {
    return b == 0 ? a : gcd(b, a % b);
}

static long lcm(long a, long b) {
    return a * b / gcd(a, b);
}

#define NAME_SIZE 3

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

typedef struct node_s {
    char name[NAME_SIZE + 1];
    char left[NAME_SIZE + 1];
    char right[NAME_SIZE + 1];
} node_t;

static int order_str(const void* s1, const void* s2) {
    return strcmp((const char*) s1, (const char*) s2);
}

static node_t* find_node(node_t* tree, int tree_len, char* name) {
    for (int i = 0; i < tree_len; i++) {
        if (strncmp(tree[i].name, name, NAME_SIZE) == 0) {
            return &tree[i];
        }
    }

    return NULL;
}

#ifdef PART2
// For part2, starting nodes are the nodes with names ending in 'A'
static int starting_nodes(node_t* tree, int tree_len, node_t* starting_nodes) {
    int starting_nodes_len = 0;

    for (int i = 0; i < tree_len; i++) {
        if (tree[i].name[NAME_SIZE - 1] == 'A') {
            memcpy(&starting_nodes[starting_nodes_len], &tree[i], sizeof(node_t));
            starting_nodes_len++;
        }
    }

    return starting_nodes_len;
}
#endif

int main() {
    char line [1000];

    FILE *file = fopen(INPUT, "r");
    if (file == NULL) {
        printf("Error: can't open file.\n");
        return 1;
    }

    char instructions[300] = {0};
    int i = 0;
    
    // too lazy to implement a linked list
    node_t tree[1000] = { 0 }; // 726
    int tree_len = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') {
            continue;
        }

        if (i == 0) {
            sscanf(line, "%s", instructions);
        } else {
            char* p = &line[0];

            node_t node;
            strncpy(node.name, p, NAME_SIZE);
            strncpy(node.left, p + 7, NAME_SIZE);
            strncpy(node.right, p + 12, NAME_SIZE);

            memcpy(&tree[tree_len], &node, sizeof(node_t));
            tree_len++;
        }

        i++;
    }

    // Not sure this makes the code more efficient
    qsort(tree, tree_len, sizeof(node_t), order_str);

    char* p = instructions;
#ifndef PART2
    long res = 0;
    node_t* n = find_node(tree, tree_len, "AAA");

    while (true) {
        res++;

        if (*p++ == 'L') {
            n = find_node(tree, tree_len, n->left);
        } else {
            n = find_node(tree, tree_len, n->right);
        }

        if (strncmp(n->name, "ZZZ", NAME_SIZE) == 0) {
            break;
        }

        if (p == &instructions[strlen(instructions)]) {
            p = instructions;
        }

    }
    printf("Part 1: %ld\n", res);
#else
    // There can be at most all the nodes - 1 starting nodes
    node_t* nodes = malloc((tree_len - 1) * sizeof(node_t));
    int nodes_len = starting_nodes(tree, tree_len, nodes);

    long res[nodes_len]; // you can't initialize a VLA
    memset(res, 0, sizeof(res)); // this line is very important

    // For each starting node (since nodes is "like" a queue, we pop the last element given that nodes_len decreases),
    // we will find the path to the end node (there is only one)
    // We keep track of the number of steps for each starting node to reach its corresponding end node
    // The total number of steps required is the LCM of all the steps
    while (true) {
        node_t* n = &nodes[nodes_len - 1];
        res[nodes_len - 1]++;

        if (*p++ == 'L') {
            n = find_node(tree, tree_len, n->left);
        } else {
            n = find_node(tree, tree_len, n->right);
        }
        memcpy(&nodes[nodes_len - 1], n, sizeof(node_t));

        if (n->name[NAME_SIZE - 1] == 'Z') {
            // printf("found %s, remaining: %d\n", n->name, nodes_len);
            nodes_len--;
        }

        if (p == &instructions[strlen(instructions)]) {
            p = instructions;
        }

        if (nodes_len == 0) {
            break;
        }
    }

    long res_lcm = 1;
    for (long i = 0; i < sizeof(res) / sizeof(res[0]); ++i) {
        res_lcm = lcm(res_lcm, res[i]);
    }
    printf("Part 2: %ld\n", res_lcm);

    free(nodes);
#endif


    fclose(file);
}
