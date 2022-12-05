#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "util.h"

void write_to_log(message_t msg) {
    FILE *log = fopen("incendios.log", "a");
    struct tm *tm_struct = localtime(&(msg.time));
    int hr = tm_struct->tm_hour;
    int min = tm_struct->tm_min;
    int sec = tm_struct->tm_sec;
    fprintf(
        log,
        "incendio detectado pelo nó %lu, na posição (%d, %d), às %02d:%02d:%02d\n",
        msg.id, msg.pos.x, msg.pos.y, hr, min, sec
    );
    fclose(log);
}

int msg_compare(void *a, void *b) {
    message_t *msg_a = (message_t *) a; 
    message_t *msg_b = (message_t *) b;
    long time_diff = msg_a->time - msg_b->time;
    if (msg_a->id == msg_b->id &&
        (time_diff < 3 || time_diff > -3) &&
        msg_a->pos.x == msg_b->pos.x &&
        msg_a->pos.y == msg_b->pos.y) {
            return 0;
    }
    return 1;
}