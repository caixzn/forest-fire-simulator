#ifndef QUEUE_H
#define QUEUE_H

// cada elemento da fila armazena seus dados e um ponteiro para o proximo da fila
typedef struct queue_node {
    void *data;
    struct queue_node* next;
} queue_node;

typedef struct queue_node* queue_iter;

// a fila armazena um ponteiro para o primeiro da fila e um para o ultimo
// (front == back), significa que tem apenas 1 elemento na fila
// (front == NULL && back == NULL) quer dizer que a fila está vazia
typedef struct queue {
    queue_iter front, back;
} queue;

queue_iter make_node(void *data, size_t sz);
void queue_push_back(queue* q, queue_iter node);
void queue_pop_front(queue* q);
queue* new_queue(void);
int queue_empty(queue *q);


#endif
