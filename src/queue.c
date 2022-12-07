#include <stdlib.h>
#include <string.h>
#include "queue.h"

/**
 * @brief Cria um novo elemento para inserir na fila
 * 
 * @param data Dados a serem armazenados
 * @param sz Tamanho dos dados em BYTES
 * @return queue_iter
 */
queue_iter make_node(void *data, size_t sz) {
    queue_iter node = malloc(sizeof(queue_node));
    node->data = malloc(sz);
    memcpy(node->data, data, sz);
    node->next = NULL;
    return node;
}

/**
 * @brief Verifica se a fila está vazia
 * 
 * @param q Ponteiro para a fila
 * 
 * @return 1 se a fila está vazia, 0 caso contrário
 */
int queue_empty(queue *q) {
    return (q->front == q->back && q->back == NULL);
}

/**
 * @brief Insere um elemento no final da fila
 * 
 * @param q Ponteiro para a fila
 * @param node Elemento à ser inserido
 */
void queue_push_back(queue *q, queue_iter node) {
    // caso a fila estiver vazia, o elemento inserido será o primeiro e
    // o último da fila simultaneamente
    if (q->front == q->back && q->back == NULL) {
        q->front = node;
    }
    // se a fila não estiver vazia, o último da fila apontará para o novo elemento
    else if (q->back != NULL) {
        q->back->next = node;
    }
    // novo elemento é o último da fila
    q->back = node;
}

/**
 * @brief Remove o primeiro da fila
 * 
 * @param q Ponteiro para a fila
 */
void queue_pop_front(queue* q) {
    queue_iter aux = q->front;
    q->front = aux->next;

    // se o próximo da fila era NULL, a fila está vazia agora
    if (q->front == NULL) {
        q->back = NULL;
    }

    free(aux->data);
    free(aux);
}

/**
 * @brief Cria uma nova fila vazia
 * 
 * @return queue* 
 */
queue* new_queue(void) {
    queue* q = malloc(sizeof(queue));
    q->back = NULL;
    q->front = NULL;
    return q;
}
