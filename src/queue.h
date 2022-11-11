#ifndef QUEUE_H
#define QUEUE_H

#include "structs.h"

typedef struct queue_node {
    position* pst;
    struct queue_node* next;
}queue_node;

typedef struct queue {
    queue_node* head;
    queue_node* tail;
    size_t size;
}queue;

void remove_head(queue* l);
void append(queue* l);
void queue_initialize(queue** l);
void queue_delete(queue** l);

#endif
