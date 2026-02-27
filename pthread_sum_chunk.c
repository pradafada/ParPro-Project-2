#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define ARRAY_SIZE 10000
#define CHUNK_SIZE 10

int a[ARRAY_SIZE];
int global_index = 0;
long long global_sum = 0;

pthread_mutex_t mutex;

void* sum_array(void* arg) {
    while (1) {

        int start, end;

        // ---- Critical Section ----
        pthread_mutex_lock(&mutex);

        if (global_index >= ARRAY_SIZE) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        start = global_index;
        end = start + CHUNK_SIZE;

        if (end > ARRAY_SIZE)
            end = ARRAY_SIZE;

        global_index = end;

        pthread_mutex_unlock(&mutex);
        // ---- End Critical Section ----

        // Local sum (outside lock)
        long long local_sum = 0;
        for (int i = start; i < end; i++) {
            local_sum += a[i];
        }

        // Add local sum to global_sum
        pthread_mutex_lock(&mutex);
        global_sum += local_sum;
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

    // Initialize array
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

        double start_time = get_time();

        for (int i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, sum_array, NULL);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        double end_time = get_time();
        double total_time = end_time - start_time;

        printf("%d\t%lld\t\t%f\n", num_threads, global_sum, total_time);

        pthread_mutex_destroy(&mutex);
    }

    return 0;
}
