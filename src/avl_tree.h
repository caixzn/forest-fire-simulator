#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>

/**
 * @brief Nodes of the AVL tree. It stores
 * the height, the quantity of nodes below 
 * it (weight) and the element choosen.
 * 
 */
typedef struct avl_node{
    __uint64_t element;
    int weight;
    int height;
    struct avl_node* right;
    struct avl_node* left;
}avl_node;

typedef struct avl_tree{
    struct avl_node* root;
    int size;
}avl_tree;

void avl_initialize(avl_tree** v);
int search_element(avl_node* node, int k);
void avl_insert(avl_tree* tree, int element);
void avl_remove(avl_tree* tree, int element);
void avl_terminate(avl_tree** v);

#endif