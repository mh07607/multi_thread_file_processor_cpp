#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

int *data = NULL;
long long int minimum = (int)INFINITY;
long long int maximum = (int)-INFINITY;
pthread_mutex_t mutex;

void *calculate_sum(void *args) {
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int *args_int = (int *)args;
    int start = args_int[0];
    int end = args_int[1];
    int thread_number = args_int[2];
    long long int partial_sum = 0;

    for (int j = start; j < end; j++) {
        partial_sum += data[j];
    }

    free(args);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                          (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Thread %d: Partial Sum = %I64i, Time taken to compute partial sum = %f.\n", thread_number, partial_sum, elapsed_time);
    return (void *)partial_sum;
}

void *find_min_and_max(void *args) {
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int *args_int = (int *)args;
    int start = args_int[0];
    int end = args_int[1];
    int thread_number = args_int[2];

    long long int local_minimum = (int)INFINITY;
    long long int local_maximum = (int)-INFINITY;
    for (int j = start; j < end; j++) {
        if (data[j] < local_minimum) {
            local_minimum = data[j];
        }
        if (data[j] > local_maximum) {
            local_maximum = data[j];
        }
    }

    pthread_mutex_lock(&mutex);
    if (local_minimum < minimum) {
        minimum = local_minimum;
    }
    if (local_maximum > maximum) {
        maximum = local_maximum;
    }
    pthread_mutex_unlock(&mutex);

    free(args);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                          (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Thread %d: Time taken to find min and max = %f.\n", thread_number, elapsed_time);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file> [num_threads]\n", argv[0]);
        return 1;
    }

    FILE *fstream = fopen(argv[1], "r");
    if (fstream == NULL) {
        perror("Error opening file");
        return 1;
    }

    pthread_mutex_init(&mutex, NULL);

    printf("Running...\n");

    int i = 0;
    int temp;
    while (fscanf(fstream, "%d", &temp) == 1) {
        data = realloc(data, (i + 1) * sizeof(int));
        if (data == NULL) {
            perror("Error reallocating memory");
            fclose(fstream);
            return 1;
        }
        data[i] = temp;
        i++;
    }

    if (fclose(fstream) != 0) {
        perror("Error closing file");
        return 1;
    }

    clock_t start_time = clock();
    long long int sum = 0;
    int thread_count = 4;
    if (argc > 2) {
        thread_count = atoi(argv[2]);
    }
    int data_per_thread = (int)i / thread_count;
    pthread_t *partial_sum_threads = malloc(thread_count * sizeof(pthread_t));
    pthread_t *min_max_threads = malloc(thread_count * sizeof(pthread_t));

    if (partial_sum_threads == NULL || min_max_threads == NULL) {
        perror("Error allocating memory for threads");
        free(data);
        pthread_mutex_destroy(&mutex);
        return 1;
    }

    // Getting partial sums
    for (int j = 0; j < thread_count; j++) {
        int *args = malloc(3 * sizeof(int));
        if (args == NULL) {
            perror("Error allocating memory for thread arguments");
            free(data);
            free(partial_sum_threads);
            free(min_max_threads);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
        args[0] = j * data_per_thread;
        args[1] = (j + 1) * data_per_thread;
        args[2] = j;
        if (pthread_create(&partial_sum_threads[j], NULL, calculate_sum, (void *)args) != 0) {
            perror("Error creating thread");
            free(data);
            free(args);
            free(partial_sum_threads);
            free(min_max_threads);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
    }

    for (int j = 0; j < thread_count; j++) {
        void *return_value;
        if (pthread_join(partial_sum_threads[j], &return_value) != 0) {
            perror("Error joining thread");
            free(data);
            free(partial_sum_threads);
            free(min_max_threads);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
        sum += (long long int)return_value;
    }

    // Getting min and max
    for (int j = 0; j < thread_count; j++) {
        int *args = malloc(3 * sizeof(int));
        if (args == NULL) {
            perror("Error allocating memory for thread arguments");
            free(data);
            free(partial_sum_threads);
            free(min_max_threads);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
        args[0] = j * data_per_thread;
        args[1] = (j + 1) * data_per_thread;
        args[2] = j;
        if (pthread_create(&min_max_threads[j], NULL, find_min_and_max, (void *)args) != 0) {
            perror("Error creating thread");
            free(data);
            free(args);
            free(partial_sum_threads);
            free(min_max_threads);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
    }

    for (int j = 0; j < thread_count; j++) {
        if (pthread_join(min_max_threads[j], NULL) != 0) {
            perror("Error joining thread");
            free(data);
            free(partial_sum_threads);
            free(min_max_threads);
            pthread_mutex_destroy(&mutex);
            return 1;
        }
    }

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Total Sum: %I64i, Minimum: %I64i, Maximum: %I64i.\nTotal Time: %f seconds.", sum, minimum, maximum, elapsed_time);

    pthread_mutex_destroy(&mutex);
   
    free(partial_sum_threads);
    free(min_max_threads);
    free(data);
    return 0;
}