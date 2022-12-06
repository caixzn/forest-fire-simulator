#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Nodes of the AVL tree. It stores
 * the height, the quantity of nodes below 
 * it (weight) and the element choosen.
 * 
 */
typedef struct avl_node{
    uint32_t element;
    int height;
    struct avl_node* right;
    struct avl_node* left;
}avl_node;

typedef struct avl_tree{
    struct avl_node* root;
    int size;
}avl_tree;

void avl_initialize(avl_tree** v);
int avl_tree_find(avl_tree* t, avl_node *v, uint32_t element)
void avl_insert(avl_tree* tree, uint32_t element);
void avl_remove(avl_tree* tree, uint32_t element);
void avl_terminate(avl_tree** v);

#endif