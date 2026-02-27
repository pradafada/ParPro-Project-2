#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define ARRAY_SIZE 10000

int a[ARRAY_SIZE];
int global_index = 0;
long long global_sum = 0;

pthread_mutex_t mutex;

void* sum_array(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        if (global_index >= ARRAY_SIZE) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        int index = global_index;
        global_index++;
        global_sum += a[index];

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec / 1e6;
}

int main() {
    // Initialize array a[i] = 2*i
    for (int i = 0; i < ARRAY_SIZE; i++) {
        a[i] = 2 * i;
    }

    printf("Threads\tGlobal_Sum\tRunning_Time (seconds)\n");
    printf("-------------------------------------------------\n");

    for (int num_threads = 1; num_threads <= 10; num_threads++) {

        pthread_t threads[num_threads];
        global_index = 0;
        global_sum = 0;
        pthread_mutex_init(&mutex, NULL);

        double start = get_time();

        // Create threads
        for (int i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, sum_array, NULL);
        }

        // Join threads
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        double end = get_time();
        double total_time = end - start;

        printf("%d\t%lld\t\t%f\n", num_threads, global_sum, total_time);

        pthread_mutex_destroy(&mutex);
    }

    return 0;
}