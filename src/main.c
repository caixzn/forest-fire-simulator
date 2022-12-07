#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "util.h"
#include "queue.h"
#include "avl_tree.h"

pthread_mutex_t map_lock = PTHREAD_MUTEX_INITIALIZER;
map_t map[GRID_SZ][GRID_SZ];
pthread_t threads[GRID_SZ/3][GRID_SZ/3];
queue *messages[GRID_SZ/3][GRID_SZ/3];
pthread_mutex_t msg_lock[GRID_SZ/3][GRID_SZ/3];
queue *central_alerts;
pthread_mutex_t central_lock = PTHREAD_MUTEX_INITIALIZER;

void map_init(void);
void msg_init(void);
void *fire_spread(void *data);
void *sensor_node(void *data);
void *firefighter(void *data);
void *display_map(void *data);
void *central_thread(void *data);

int main(void) {
    msg_init();
    map_init();
    pthread_t central, fire, display;
    pthread_create(&central, NULL, central_thread, NULL);
    pthread_create(&fire, NULL, fire_spread, NULL);
    pthread_create(&display, NULL, display_map, NULL);
    pthread_join(display, NULL);
    // the program never terminates itself
}

void msg_init(void) {
    for (int i = 0; i < GRID_SZ/3; ++i) {
        for (int j = 0; j < GRID_SZ/3; ++j) {
            messages[i][j] = new_queue();
            pthread_mutex_init(&msg_lock[i][j], NULL);
        }
    }
}

void map_init(void) {
    for (int i = 0; i < GRID_SZ; ++i) {
        for (int j = 0; j < GRID_SZ; ++j) {
            // grass
            map[i][j].state = 'w';
            // this is to prevent the same fire from being reported many times
            map[i][j].burning = 0;
            // sensor node
            if (i%3 == 1 && j%3 == 1) {
                map[i][j].state = 'T';
                coord_t *pos = (coord_t *)malloc(sizeof(coord_t));
                pos->x = i;
                pos->y = j;
                pthread_create(&threads[i/3][j/3], NULL, sensor_node, (void *) pos);
            }
        }
    }
}

void *msg_remove(void *data) {
    sleep(30);
    char **matrix = (char **) data;
    free(matrix[0]);
    free(matrix);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

void *fire_spread(void *data) {
    srand(time(NULL));
    while (1) {
        sleep(5);
        int x = random() % GRID_SZ;
        int y = random() % GRID_SZ;
        pthread_mutex_lock(&map_lock);
        map[x][y].state = '@';
        pthread_mutex_unlock(&map_lock);
    }
}

void *sensor_node(void *data) {
    coord_t pos = *(coord_t *)data;
    int x = pos.x, y = pos.y;
    int id_x = x/3, id_y = y/3;
    free(data);

    queue *msgs_to_send = new_queue();

    while (map[x][y].state == 'T') {
        pthread_mutex_lock(&map_lock);
        for (int i = x-1; i <= x+1; ++i) {
            for (int j = y-1; j <= y+1; ++j) {
                if (map[i][j].burning == 1)
                    continue;
                if (map[i][j].state == '@') {
                    map[i][j].burning = 1;
                    message_t *msg = malloc(sizeof(message_t));
                    msg->time = time(NULL);
                    msg->id = threads[id_x][id_y];
                    msg->pos.x = i;
                    msg->pos.y = j;
                    msg->message_id = random();
                    msg->visited = malloc(sizeof(char *)*(GRID_SZ/3));
                    msg->visited[0] = calloc((GRID_SZ/3)*(GRID_SZ/3), sizeof(char));
                    for (size_t i = 1; i < GRID_SZ/3; i++)
                        msg->visited[i] = &(msg->visited[0][i*(GRID_SZ/3)]);
                    msg->visited[id_x][id_y] = 1;
                    pthread_t thread_id;

                    // starts timer to delete message
                    pthread_create(&thread_id, NULL, msg_remove, (void*) (msg->visited));

                    queue_push_back(msgs_to_send, make_node((void *)msg, sizeof(message_t)));
                    free(msg);
                }
            }
        }
        pthread_mutex_unlock(&map_lock);

        pthread_mutex_lock(&msg_lock[id_x][id_y]);
        if (!queue_empty(messages[id_x][id_y])) {
            while (!queue_empty(messages[id_x][id_y])) {
                queue_iter it = messages[id_x][id_y]->front;
                queue_push_back(msgs_to_send, make_node(it->data, sizeof(message_t)));
                queue_pop_front(messages[id_x][id_y]);
            }
        }
        pthread_mutex_unlock(&msg_lock[id_x][id_y]);

        if (!queue_empty(msgs_to_send)) {
            // neighbor nodes
            coord_t coords[] = {
                {id_x - 1, id_y},
                {id_x + 1, id_y},
                {id_x, id_y - 1},
                {id_x, id_y + 1}
            };

            // check if node is on the border
            // if it is, communicate with central thread
            if (id_x - 1 < 0 || id_y - 1 < 0 || id_x + 1 >= GRID_SZ/3 ||
                id_y + 1 >= GRID_SZ/3) {
                pthread_mutex_lock(&central_lock);
                while (!queue_empty(msgs_to_send)) {
                    queue_push_back(
                        central_alerts,
                        make_node(msgs_to_send->front->data, sizeof(message_t))
                    );
                    queue_pop_front(msgs_to_send);
                }
                pthread_mutex_unlock(&central_lock);
            }
            else {
                for (int i = 0; i < 4; ++i) {
                    int tmp_x = coords[i].x, tmp_y = coords[i].y;
                    pthread_mutex_lock(&map_lock);
                    if (map[x][y].state != 'T'){
                        pthread_mutex_unlock(&map_lock);
                        continue;
                    }
                    pthread_mutex_unlock(&map_lock);

                    queue_iter it = msgs_to_send->front;
                    pthread_mutex_lock(&msg_lock[tmp_x][tmp_y]);
                    while (it != NULL) {
                        message_t *m = (message_t *)it->data;
                        if (!m->visited[tmp_x][tmp_y]){
                            m->visited[tmp_x][tmp_y] = 1;
                            queue_push_back(
                                messages[tmp_x][tmp_y],
                                make_node(it->data, sizeof(message_t))
                            );
                        }
                        it = it->next;
                    }
                    pthread_mutex_unlock(&msg_lock[tmp_x][tmp_y]);
                }

                while (!queue_empty(msgs_to_send))
                    queue_pop_front(msgs_to_send);
            }
        }

        sleep(1);
    }

    // clean up when node is destroyed
    pthread_mutex_lock(&msg_lock[id_x][id_y]);
    while (!queue_empty(messages[id_x][id_y]))
        queue_pop_front(messages[id_x][id_y]);
    pthread_mutex_unlock(&msg_lock[id_x][id_y]);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

void *central_thread(void *data) {
    central_alerts = new_queue();

    avl_tree *message_ids;
    avl_initialize(&message_ids);

    while (1) {
        sleep(1);
        pthread_mutex_lock(&central_lock);
        while (!queue_empty(central_alerts)) {
            queue_iter new = central_alerts->front;
            message_t new_msg = *(message_t *)new->data;
            if (!avl_tree_find(message_ids, message_ids->root, new_msg.message_id)){
                avl_insert(message_ids, new_msg.message_id);
                pthread_t thread_id;
                pthread_create(&thread_id, NULL, firefighter, &map[new_msg.pos.x][new_msg.pos.y]);
                write_to_log(new_msg);
            }
            queue_pop_front(central_alerts);
        }
        pthread_mutex_unlock(&central_lock);
    }
    avl_terminate(&message_ids);
}

void *firefighter(void *data) {
    map_t *state = (map_t *) data;
    sleep(2);
    pthread_mutex_lock(&map_lock);
    state->state = '_';
    state->burning = 0;
    pthread_mutex_unlock(&map_lock);

    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

void *display_map(void *data) {
    // background color
    printf("\e[48;5;106m");
    // clears the terminal screen
    system("tput clear");

    while (1) {
        pthread_mutex_lock(&map_lock);
        for (int i = 0; i < GRID_SZ; ++i) {
            for (int j = 0; j < GRID_SZ; ++j) {
                int color = 64; // green
                switch (map[i][j].state) {
                    case '@':
                        color = 202; // orange
                        break;
                    case 'T':
                        color = 232; // dark gray
                        break;
                    case '_':
                        color = 245; // light gray
                        break;
                    default:
                        break;
                }
                printf("\e[38;5;%dm", color);
                printf("%2c", map[i][j].state);
            }
            puts(" ");
        }
        // moves the cursor back to the beginning
        printf("\e[H");
        pthread_mutex_unlock(&map_lock);
        sleep(1);
    }
}
