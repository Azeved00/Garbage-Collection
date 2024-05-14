#ifndef ASCII_TREE_H
#define ASCII_TREE_H

#include "bistree.h"

// Struct for storing the details of nodes in an ASCII form for printing
typedef struct asciinode_struct asciinode;

struct asciinode_struct {
    asciinode *left, *right;
    int edge_length;   // Length of the edge from this node to its children
    int height;        // Height of the node
    int lablen;        // Length of the label
    int parent_dir;    // Direction indicator: -1 = left, 0 = root, 1 = right
    char label[11];    // Node value in string form, max length 10 + 1 for null terminator
};

// Functions for managing ASCII tree printing
asciinode *build_ascii_tree(BiTreeNode *t);
void free_ascii_tree(asciinode *node);
void print_ascii_tree(BiTreeNode *t);
BiTreeNode *find_min(BiTreeNode *t);

#endif /* ASCII_TREE_H */
