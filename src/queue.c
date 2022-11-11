#include <stdlib.h>

#include "util.h"
#include "queue.h"

// Create new node
static queue_node* queue_new_node(void){
    queue_node* new_node;
    new_node = mallocx(sizeof(queue_node));
    position* pst = mallocx(sizeof(position));

    new_node->pst = pst;
    new_node->next = NULL;
    return new_node;
}

// Remove first item of the queue
void remove_head(queue* l){
    queue_node* node = l->head;

    if (l->size == 1)
        l->tail = NULL;

    l->head = l->head->next;
    l->size--;

    free(node->pst);
    free(node);
    return;
}

// Add item to the end of the queue
void append(queue* l){
    queue_node* new_node = queue_new_node();

    if (l->size == 0)
        l->head = new_node;
    else 
        l->tail->next = new_node;

    l->tail = new_node;
    l->size++;
    return;
}

// Initialize queue
void queue_initialize(queue** l){
    // Allocate memory
    (*l) = mallocx(sizeof(queue));
    // Make head and tail pointing to NULL
    (*l)->head = NULL;
    (*l)->tail = NULL;
    // Make size 0
    (*l)->size = 0;
    return;
}

// Delete all items in queue and the queue itself
void queue_delete(queue** l){
    // Remove all nodes first and remove queue after
    while((*l)->size != 0)
        remove_head(*l);
    free(*l);
    *l = NULL;
    return;
}