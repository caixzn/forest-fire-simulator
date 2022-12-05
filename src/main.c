#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "util.h"
#include "queue.h"

#define GRID_SZ 30

pthread_mutex_t map_lock = PTHREAD_MUTEX_INITIALIZER;
char map[GRID_SZ][GRID_SZ];
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
            map[i][j] = 'w';
            // sensor node
            if (i%3 == 1 && j%3 == 1) {
                map[i][j] = 'T';
                coord_t *pos = (coord_t *)malloc(sizeof(coord_t));
                pos->x = i;
                pos->y = j;
                pthread_create(&threads[i/3][j/3], NULL, sensor_node, (void *) pos);
            }
        }
    }
}

void *fire_spread(void *data) {
    srand(time(NULL));
    while (1) {
        sleep(5);
        int x = random() % GRID_SZ;
        int y = random() % GRID_SZ;
        pthread_mutex_lock(&map_lock);
        map[x][y] = '@';
        pthread_mutex_unlock(&map_lock);
    }
}

void *sensor_node(void *data) {
    coord_t pos = *(coord_t *)data;
    int x = pos.x, y = pos.y;
    int id_x = x/3, id_y = y/3;
    free(data);

    queue *msgs_to_send = new_queue();

    while (map[x][y] == 'T') {
        pthread_mutex_lock(&map_lock);
        for (int i = x-1; i <= x+1; ++i) {
            for (int j = y-1; j <= y+1; ++j) {
                if (map[i][j] == '@') {
                    message_t *msg = malloc(sizeof(message_t));
                    msg->time = time(NULL);
                    msg->id = threads[id_x][id_y];
                    msg->pos.x = i;
                    msg->pos.y = j;
                    msg->prev_sender = msg->id;
                    queue_push_back(msgs_to_send, make_node((void *)msg, sizeof(message_t)));
                }
            }
        }
        pthread_mutex_unlock(&map_lock);

        if (!queue_empty(messages[id_x][id_y])) {
            pthread_mutex_lock(&msg_lock[id_x][id_y]);
            while (!queue_empty(messages[id_x][id_y])) {
                queue_iter it = messages[id_x][id_y]->front;
                message_t *m = (message_t *)it->data;
                m->prev_sender = threads[id_x][id_y];
                queue_pop_front(messages[id_x][id_y]);
                queue_push_back(msgs_to_send, make_node((void *)m, sizeof(message_t)));
            }
            pthread_mutex_unlock(&msg_lock[id_x][id_y]);
        }

        if (!queue_empty(msgs_to_send)) {
            coord_t coords[] = {
                {id_x - 1, id_y},
                {id_x + 1, id_y},
                {id_x, id_y - 1},
                {id_x, id_y + 1}
            };
            if (id_x - 1 < 0 || id_y - 1 < 0 || id_x + 1 >= GRID_SZ/3 ||
                id_y + 1 >= GRID_SZ/3) {
                pthread_mutex_lock(&central_lock);
                while (!queue_empty(msgs_to_send)) {
                    queue_push_back(central_alerts, make_node(msgs_to_send->front->data, sizeof(message_t)));
                    queue_pop_front(msgs_to_send);
                }
                pthread_mutex_unlock(&central_lock);
            }
            else {
                for (int i = 0; i < 4; ++i) {
                    int tmp_x = coords[i].x, tmp_y = coords[i].y;
                    queue_iter it = msgs_to_send->front;
                    pthread_mutex_lock(&msg_lock[tmp_x][tmp_y]);
                    while (it != NULL) {
                        message_t *m = (message_t *)it->data;
                        if (threads[tmp_x][tmp_y] != m->prev_sender)
                            queue_push_back(messages[tmp_x][tmp_y], make_node(it->data, sizeof(message_t)));
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

    pthread_mutex_lock(&msg_lock[id_x][id_y]);
    while (!queue_empty(messages[id_x][id_y]))
        queue_pop_front(messages[id_x][id_y]);
    pthread_mutex_unlock(&msg_lock[id_x][id_y]);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

void *central_thread(void *data) {
    central_alerts = new_queue();

    queue* written_to_log = new_queue();

    while (1) {
        if (queue_empty(central_alerts)) {
            sleep(1);
            continue;
        }
        pthread_mutex_lock(&central_lock);
        while (!queue_empty(central_alerts)) {
            queue_iter new = central_alerts->front;
            queue_iter it = written_to_log->front;
            int write_this = 1;
            while (it != NULL) {
                if (!msg_compare(new->data, it->data)) {
                    write_this = 0;
                    break;
                }
                it = it->next;
            }
            if (write_this) {
                pthread_t thread_id;
                message_t new_msg = *(message_t *)new->data;
                pthread_create(&thread_id, NULL, firefighter, &map[new_msg.pos.x][new_msg.pos.y]);
                queue_push_back(written_to_log, make_node(new->data, sizeof(message_t)));
                write_to_log(new_msg);
            }
            queue_pop_front(central_alerts);
        }
        pthread_mutex_unlock(&central_lock);
    }
}

void *firefighter(void *data) {
    char *state = (char *) data;
    sleep(2);
    pthread_mutex_lock(&map_lock);
    *state = '_';
    pthread_mutex_unlock(&map_lock);
    pthread_exit(NULL);
}

void *display_map(void *data) {
    // clears the terminal screem
    system("tput clear");
    // background color
    printf("\e[48;5;106m");
    while (1) {
        pthread_mutex_lock(&map_lock);
        for (int i = 0; i < GRID_SZ; ++i) {
            for (int j = 0; j < GRID_SZ; ++j) {
                int color = 64; // green
                switch (map[i][j]) {
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
                printf("%2c", map[i][j]);
            }
            puts(" ");
        }
        // moves the cursor back to the beginning
        printf("\e[H");
        pthread_mutex_unlock(&map_lock);
        sleep(1);
    }
}
