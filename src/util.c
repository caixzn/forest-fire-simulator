#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "util.h"

// Function to allocate memory and verify it
void* mallocx(const unsigned int size){
    void* alloc;
    alloc = malloc(size);
    if (alloc == NULL){
        printf("Allocation error\n");
        exit(EXIT_FAILURE);
    }
    return alloc;
}

// Convert time in int to string
char* time_to_hour(char* time_string){
    time_t current_time;
    struct tm * time_info;

    time(&current_time);
    time_info = localtime(&current_time);
    sprintf(time_string, "%02d:%02d:%02d", 
            time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
    return time_string;
}