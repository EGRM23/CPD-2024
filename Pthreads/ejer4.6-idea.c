#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

pthread_cond_t read_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t write_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;

int read_count = 0;
int write_queue = 0;
int write_flag = 0;

void start_read() {
    pthread_mutex_lock(&rw_mutex);

    // Prioridad a escritores
    while (write_flag || write_queue > 0) {
        pthread_cond_wait(&read_cond, &rw_mutex);
    }

    read_count++;
    pthread_mutex_unlock(&rw_mutex);
}

void end_read() {
    pthread_mutex_lock(&rw_mutex);
    read_count--;
    if (read_count == 0) {
        pthread_cond_signal(&write_cond);
    }
    pthread_mutex_unlock(&rw_mutex);
}

void start_write() {
    pthread_mutex_lock(&rw_mutex);
    write_queue++;
    while (write_flag || read_count > 0) {
        pthread_cond_wait(&write_cond, &rw_mutex);
    }
    write_queue--;
    write_flag = 1;
    pthread_mutex_unlock(&rw_mutex);
}

void end_write() {
    pthread_mutex_lock(&rw_mutex);
    write_flag = 0;

    // Prioridad a los escritores
    if (write_queue > 0) {
        pthread_cond_signal(&write_cond);
    } else {
        pthread_cond_broadcast(&read_cond);
    }
    pthread_mutex_unlock(&rw_mutex);
}

int main(int argc, char* argv[])
{
    
}