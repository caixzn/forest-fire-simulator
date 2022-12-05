#ifndef UTIL_H
#define UTIL_H

#include <time.h>
#include <pthread.h>

typedef struct coord_t {
    int x, y;
} coord_t;

typedef struct message_t {
    time_t time;
    pthread_t id;
    pthread_t prev_sender;
    coord_t pos;
} message_t;

void write_to_log(message_t msg);
int msg_compare(void *a, void *b);

#endif