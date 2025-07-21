#include "sni.h"

#include <stdlib.h>
#include <string.h>

struct us_sni_node {
    char *hostname;
    void *user;
    struct us_sni_node *next;
};

struct us_sni_tree {
    struct us_sni_node *first;
};

struct us_sni_tree *sni_new() {
    return calloc(1, sizeof(struct us_sni_tree));
}

void sni_free(struct us_sni_tree *tree) {
    struct us_sni_node *node = tree->first;
    while (node) {
        struct us_sni_node *next = node->next;
        free(node->hostname);
        free(node);
        node = next;
    }
    free(tree);
}

void sni_add(struct us_sni_tree *tree, const char *hostname, void *user) {
    struct us_sni_node *node = malloc(sizeof(struct us_sni_node));
    node->hostname = strdup(hostname);
    node->user = user;
    node->next = tree->first;
    tree->first = node;
}

void *sni_find(struct us_sni_tree *tree, const char *hostname) {
    struct us_sni_node *node = tree->first;
    while (node) {
        if (!strcmp(node->hostname, hostname)) {
            return node->user;
        }
        node = node->next;
    }
    return NULL;
}

void sni_remove(struct us_sni_tree *tree, const char *hostname) {
    struct us_sni_node **p_node = &tree->first;
    while (*p_node) {
        struct us_sni_node *node = *p_node;
        if (!strcmp(node->hostname, hostname)) {
            *p_node = node->next;
            free(node->hostname);
            free(node);
            return;
        }
        p_node = &node->next;
    }
}
