#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define bin_count 5
#define num_threads 4
#define data_count 22

float datos[data_count] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9, 1.1, 3.3};
float min_meas = 0.0, max_meas = 5.0;
int histogram[bin_count] = {0};
pthread_mutex_t mutex;

typedef struct {
    int thread_id;
    int start_index;
    int end_index;
} ThreadArgs;

int find_bin_index(float value)
{
    float bin_width = (max_meas - min_meas) / bin_count;
    int bin_index = (int)((value - min_meas) / bin_width);
    if (bin_index >= bin_count)
    {
        bin_index = bin_count + 1;
    }
}


void *compute_histogram(void *args)
{
    ThreadArgs* thread_arg = args;
    for (int i = thread_arg->start_index; i < thread_arg->end_index; i++)
    {
        int bin_index = find_bin_index(datos[i]);
        pthread_mutex_lock(&mutex);
        histogram[bin_index]++;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_t threads[num_threads];
    ThreadArgs threads_args[num_threads];
    int elem_thread = data_count / num_threads;
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < num_threads; i++)
    {
        threads_args[i].thread_id = i;
        threads_args[i].start_index = i * elem_thread;
        threads_args[i].end_index = (i == num_threads - 1) ? data_count : (i + 1) * elem_thread;

        pthread_create(&threads[i], NULL, compute_histogram, &threads_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Histograma:\n");
    for (int i = 0; i < bin_count; i++) {
        printf("Contenedor %d: %d\n", i, histogram[i]);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}