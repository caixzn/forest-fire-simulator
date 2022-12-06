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
