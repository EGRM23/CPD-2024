#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long total_tosses;
long long total_in_circle = 0;
int num_threads;
pthread_mutex_t mutex;

void *monte_carlo(void* number_of_tosses)
{
    double x, y, distance_squared;
    int number_in_circle = 0;
    srand(time(NULL));

    for (int toss = 0; toss < (long long) number_of_tosses; toss++) {
        x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        y = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        distance_squared = x*x + y*y;
        if (distance_squared <= 1) number_in_circle++;
    }
    
    pthread_mutex_lock(&mutex);
    total_in_circle += number_in_circle;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Uso: %s <total_tosses> <num_threads>\n", argv[0]);
        return 1;
    }

    total_tosses = atoll(argv[1]);
    num_threads = atoi(argv[2]);

    if (num_threads <= 0 || total_tosses <= 0) {
        printf("Error: total_tosses y num_threads deben ser mayores que 0.\n");
        return 1;
    }

    pthread_t threads[num_threads];
    long long elem_thread = total_tosses / num_threads;
    pthread_mutex_init(&mutex, NULL);

    for (long i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, monte_carlo, (void*)elem_thread);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    float pi_estimate = 4.0 * total_in_circle / ((double) total_tosses);
    printf("Estimación de π con %lld lanzamientos y %i hilos: %.15f\n", total_tosses, num_threads, pi_estimate);

    pthread_mutex_destroy(&mutex);
    return 0;
}