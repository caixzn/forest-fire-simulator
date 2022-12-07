#ifndef UTIL_H
#define UTIL_H

#include <time.h>
#include <pthread.h>
#include <stdint.h>

#define GRID_SZ 30

typedef struct coord_t {
    int x, y;
} coord_t;

typedef struct message_t {
    time_t time;
    pthread_t id;
    coord_t pos;
    char **visited;
    uint64_t message_id;
} message_t;

typedef struct map_t {
    char state;
    char visited;
} map_t;

void write_to_log(message_t msg);
int msg_compare(void *a, void *b);

#endif