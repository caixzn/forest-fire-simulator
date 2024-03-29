#include "avl_tree.h"

/**
 * @brief Get the height object.
 * 
 * @param node 
 * @return int 
 */
inline static int get_height(avl_node *node){
    if (node == NULL)
        return 0;
    return node->height;
}

/**
 * @brief Function to calculate the balance
 * of a certain node of the tree.
 * 
 * @param node A pointer to the node
 * @return int The balance of the node
 */
static int calculate_balance(avl_node *node) {
    if (node == NULL)
        return 0;
    return get_height(node->left) - get_height(node->right);
    
}

/**
 * @brief Calculate the height
 * of the node.
 * 
 * @param node 
 * @return int 
 */
static int calculate_height(avl_node *node) {
    if (node == NULL)
        return 0;
    int hl = get_height(node->left);
    int hr = get_height(node->right);
    return hl < hr ? hr + 1 : hl + 1;
}

/**
 * @brief Define the initial values
 * of the node.
 * 
 * @param value The element stored in 
 * the node
 * @return avl_node* 
 */
static avl_node *node_builder(uint32_t value) {
    avl_node *new_node = malloc(sizeof(avl_node));
    new_node->element = value;
    new_node->height = 0;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

/**
 * @brief Rotate a segment of the tree. 
 * Used to balance it.
 * 
 * @param root A pointer to a node
 * @return avl_node* A pointer to the 
 * rotated segment of the tree
 */
static avl_node *rotate_right(avl_node *root) {
    avl_node *middle = root->left;

    root->left = middle->right;
    middle->right = root;

    root->height = calculate_height(root);
    middle->height = calculate_height(middle);
    
    return middle;
}
/**
 * @brief Rotate a segment of the tree. 
 * Used to balance it.
 * 
 * @param root A pointer to a node
 * @return avl_node* A pointer to the 
 * rotated segment of the tree
 */
static avl_node *rotate_left(avl_node *root) {
    avl_node *middle = root->right;

    root->right = middle->left;
    middle->left = root;

    root->height = calculate_height(root);
    middle->height = calculate_height(middle);
    
    return middle;
}


/**
 * @brief Initialize AVL tree. The function 
 * allocates memory for the AVL and define 
 * the initial values of it.
 * 
 * @param v A pointer to the AVL tree
 */
void avl_initialize(avl_tree **v) {
    (*v) = malloc(sizeof(avl_tree));
    (*v)->root = NULL;
    (*v)->size = 0;
    return;
}

/**
 * @brief Recursive function to iterate 
 * through the tree until the element is found.
 * 
 * @param t Pointer to the tree
 * @param node A node of the AVL tree
 * @param element The element to be inserted
 * @return int If the value was found
 */
int avl_tree_find(avl_tree *t, avl_node *v, uint32_t element) {
    if (v == NULL) 
        return 0;
    
    if (v->element == element)
        return 1;
    else if (v->element > element)
        return avl_tree_find(t, v->left, element);
    else
        return avl_tree_find(t, v->right, element);
}

/**
 * @brief Recursive function to iterate 
 * through the tree until a leaf is reached. 
 * It balances the tree if needed.
 * 
 * @param node A node of the AVL tree
 * @param element The element to be inserted
 * @return avl_node* A pointer to the node
 */
static avl_node *add_element(avl_node *node, uint32_t element) {
    if (node == NULL)
        node = node_builder(element);
    else if (node->element > element)
        node->left = add_element(node->left, element);
    else
        node->right = add_element(node->right, element);
    
    node->height = calculate_height(node);
    int balance = calculate_balance(node);
    
    if (balance > 1) {
        if (calculate_balance(node->left) < 0)
            node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1) {
        if (calculate_balance(node->right) > 0)
            node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}
/**
 * @brief Insert elements in the AVL tree.
 * 
 * @param tree A pointer to the tree
 * @param element The element to be inserted
 */
void avl_insert(avl_tree *tree, uint32_t element) {
    tree->root = add_element(tree->root, element);
    ++tree->size;
}
/**
 * @brief Find the smallest element among 
 * the elements greater than the root.
 * 
 * @param node The right side of the root
 * @return avl_node* A pointer to the smallest
 * element
 */
static avl_node *find_leftmost(avl_node *node) {
    if (node->left == NULL)
        return node;
    return find_leftmost(node->left);
}

/**
 * @brief Remove element of the AVL tree.
 * 
 * @param node A pointer to the root
 * @param element The element to be removed 
 * @return avl_node* A pointer to the root
 */
static avl_node *remove_element(avl_node *node, uint32_t element) {
    if (node->element == element) {
        avl_node *aux = NULL;
        // Cases where the root has one child
        if (node->left == NULL) {
            aux = node->right;
            free(node);
            return aux;
        }
        if (node->right == NULL) {
            aux = node->left;
            free(node);
            return aux;
        }
        // Case where the root has two children
        aux = find_leftmost(node->right);
        // Swap values of root and leaf
        int tmp = node->element;
        node->element = aux->element;
        aux->element = tmp;
        // Remove leaf
        node->right = remove_element(node->right, element);
    }
    else if (node->element > element)
        node->left = remove_element(node->left, element);
    else
        node->right = remove_element(node->right, element);

    node->height = calculate_height(node);
    int balance = calculate_balance(node);
    
    if (balance > 1) {
        if (calculate_balance(node->left) < 0)
            node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1) {
        if (calculate_balance(node->right) > 0)
            node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}
/**
 * @brief Remove elements from the AVL tree.
 * 
 * @param tree A pointer to the tree
 * @param element The value to be removed
 */
void avl_remove(avl_tree *tree, uint32_t element) {
    tree->root = remove_element(tree->root, element);
    --tree->size;
}
/**
 * @brief Remove all elements from the
 * AVL tree.
 * 
 * @param node A node of the tree
 */
static void remove_all(avl_node *node) {
    if (node == NULL)
        return;
    remove_all(node->left);
    remove_all(node->right);
    free(node);
}
/**
 * @brief Deallocate memory used in the
 * AVL tree and its nodes.
 * 
 * @param tree The AVL tree
 */
void avl_terminate(avl_tree **tree) {
    remove_all((*tree)->root);
    free((*tree));
    (*tree) = NULL;
    return;
}