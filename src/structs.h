#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h> 
#include <stdbool.h>

#define GRID 30

typedef struct position {
    int x;
    int y;
} position;

typedef struct node {
    pthread_t thread_id;
    char state;
    bool visited;
    position pst;
} node;

typedef struct message {
    pthread_t thread_id;
    position pst;
    time_t tm;
} message;

#endif