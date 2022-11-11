#include <stdio.h>       
#include <pthread.h>     
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "queue.h"
#include "util.h"

pthread_mutex_t lock;
node** matrix;
FILE* incendios;

// Thread to print the map of the forest every second
void* update_screen(void* data){
    while(true){
        sleep(1);
        // Access critical region
        pthread_mutex_lock(&lock);
        for (int i = 0; i < GRID; i++){
            for (int j = 0; j < GRID; j++){
                if (matrix[i][j].state == 'T')
                    printf("\033[1;33m");
                else if (matrix[i][j].state == '@')
                    printf("\033[1;31m");
                else
                    printf("\033[0;32m");
                printf("%3c", matrix[i][j].state);
            }
            printf("\n");
        }
        printf("\n");
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

// Thread to put out the fire and register when it was put out.
void* fireman(void* data){
    message* msg = (message*) data;
    int x = msg->pst.x;
    int y = msg->pst.y;
    sleep(2);

    // Get actual time and send log information to RAM
    char time_string[9]; 
    char* time_aux = time_to_hour((char*)&time_string);
    fprintf(incendios, "%s - %lu - (%d, %d)\n", time_aux, msg->thread_id, x, y);
    
    // Access critical region
    pthread_mutex_lock(&lock);
    matrix[x][y].state = '-';
    matrix[x][y].visited = false;
    pthread_mutex_unlock(&lock);

    // Update log and exit
    fclose(incendios);
    incendios = fopen("incendios.log", "a");
    pthread_exit(NULL);
}

void bfs(message* msg, int x, int y){
    // Initialize queue
    queue* q;
    queue_initialize(&q);
    append(q);
    q->tail->pst->x = x;
    q->tail->pst->y = y;
    while(q->size != 0){
        queue_node* head = q->head;
        int x = head->pst->x;
        int y = head->pst->y;
        // Verify if the borders sensors are found
        if (x == 1 || y == 1){
            pthread_t aux;
            pthread_create(&aux, NULL, fireman, (void*) msg);
            break;
        }
        if (x == GRID - 2 || y == GRID - 2){
            pthread_t aux;
            pthread_create(&aux, NULL, fireman, (void*) msg);
            break;
        }
        // Verify limits of the search
        if (x < 0 || y < 0){
            remove_head(q);
            continue;
        }
        if (x > GRID || y > GRID){
            remove_head(q);
            continue;
        }
        // Verify if the sensor was destroyed
        if (matrix[x][y].state != 'T'){
            remove_head(q);
            continue;
        }
        if (matrix[x][y].visited){
            remove_head(q);
            continue;
        }
        matrix[x][y].visited = true;
        // Add neighbor sensors to the search
        append(q);
        q->tail->pst->x = x - 3;
        q->tail->pst->y = y;
        append(q);
        q->tail->pst->x = x + 3;
        q->tail->pst->y = y;
        append(q);
        q->tail->pst->x = x;
        q->tail->pst->y = y - 3;
        append(q);
        q->tail->pst->x = x;
        q->tail->pst->y = y + 3;
    }
    queue_delete(&q);
    return;
}

// Function to restart the visited sensors after
// a BFS is done.
void restart(){
    for (int i = 1; i < GRID; i += 3){
        for (int j = 1; j < GRID; j += 3){
            matrix[i][j].visited = false;
        }
    }
}

// Function to 
void* surveillance(void* data){
    node* sensor = (node*) data;
    while(true){
        sleep(1);
        // Enter critical region
        pthread_mutex_lock(&lock);
        int x = sensor->pst.x;
        int y = sensor->pst.y;
        // Verify if the sensor was not destroyed
        if (sensor->state != '@'){
            // Verify surrounds
            for (int i = x - 1; i <= x + 1; i++){
                for (int j = y - 1; j <= y + 1; j++){
                    // If there is a fire already visited, ignore
                    if (matrix[i][j].visited)
                        continue;
                    if (matrix[i][j].state == '@'){
                        // Send message to neighbor sensors
                        message aux;
                        aux.thread_id = sensor->thread_id;
                        position aux1;
                        aux1.x = i;
                        aux1.y = j;
                        aux.pst = aux1;
                        message* msg = &aux;
                        matrix[i][j].visited = true;
                        bfs(msg, x, y);
                        restart();
                    }
                }
            }
            pthread_mutex_unlock(&lock);
            continue;
        }
        matrix[x][y].thread_id = 0;
        pthread_mutex_unlock(&lock);
        break;
    }
    pthread_exit(NULL);
}

// Thread to start a fire
void* fire(void* data){
    while(true){
        sleep(5);
        int x = rand() % GRID;
        int y = rand() % GRID;
        // Access critical region
        pthread_mutex_lock(&lock);
        matrix[x][y].state = '@';
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

int main(){
    // Start log
    incendios = fopen("incendios.log", "w");
    pthread_t thread_id[2];

    // Start forest map
    matrix = mallocx(sizeof(node*) * GRID);
    for (int i = 0; i < GRID; i++)
        matrix[i] = mallocx(sizeof(node) * GRID);
    
    // Fill forest map with information needed
    for (int i = 0; i < GRID; i++){
        for (int j = 0; j < GRID; j++){
            matrix[i][j].thread_id = 0;
            matrix[i][j].state = '-';
            matrix[i][j].visited = false;
            matrix[i][j].pst.x = i;
            matrix[i][j].pst.y = j;
        }
    }

    // Create sensor threads
    for (int i = 1; i < GRID; i += 3){
        for (int j = 1; j < GRID; j += 3){
            matrix[i][j].state = 'T';
            pthread_create(&(matrix[i][j].thread_id), NULL, surveillance, 
                            (void*) &matrix[i][j]);
        }
    }

    srand(time(NULL));
    pthread_create(&thread_id[0], NULL, update_screen, NULL);
    pthread_create(&thread_id[1], NULL, fire, NULL);
    
    for (int i = 0; i < 2; i++)
        pthread_join(thread_id[i], NULL);
    

    for (int i = 0; i < GRID; i++)
        free(matrix[i]);
    free(matrix);
    return 0;
}