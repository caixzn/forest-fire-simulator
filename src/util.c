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

void write_debug(fire_t fire, time_t t, int aux) {
    FILE *debug = fopen("debug.log", "a");
    struct tm *tm_struct = localtime(&(fire.time));
    int hr = tm_struct->tm_hour;
    int minl = tm_struct->tm_min;
    int sec = tm_struct->tm_sec;
    struct tm *tm_structt = localtime(&(t));
    int thr = tm_structt->tm_hour;
    int tmin = tm_structt->tm_min;
    int tsec = tm_structt->tm_sec;
    fprintf(debug, "**************************************************************\n");
    fprintf(debug,"Tempo para espalhamento: %d\n", aux);
    fprintf(debug,"Tempo Atual: %02d:%02d:%02d\n", thr, tmin, tsec);
    fprintf(debug,"Tempo do Fogo: %02d:%02d:%02d\n", hr, minl, sec);
    fprintf(debug,"x = %d  y = %d\n", fire.pos.x, fire.pos.y);
    
    fclose(debug);
}

int min(int a, int b) {
    return a < b ? a : b;
}
