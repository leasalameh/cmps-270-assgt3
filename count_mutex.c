/*
https://github.com/leasalameh/cmps-270-assgt3
username: leasalameh, repo: cmps-270-assgt3

1,000 integers, 1 thread: Elapsed time: 0.000000 seconds
1,000 integers, 2 threads: Elapsed time: 0.001000 seconds
1,000 integers, 4 threads: Elapsed time: 0.001000 seconds
1,000 integers, 32 threads: Elapsed time: 0.001000 seconds

1,000,000 integers, 1 thread: Elapsed time: 0.004000 seconds
1,000,000 integers, 2 threads: Elapsed time: 0.002000 seconds
1,000,000 integers, 4 threads: Elapsed time: 0.002000 seconds
1,000,000 integers, 32 threads: Elapsed time: 0.002000 seconds

1,000,000,000 integers, 1 thread: Elapsed time: 2.486000 seconds
1,000,000,000 integers, 2 threads: Elapsed time: 1.309000 seconds
1,000,000,000 integers, 4 threads: Elapsed time: 0.816000 seconds
1,000,000,000 integers, 32 threads: Elapsed time: 0.714000 seconds

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int *array;
int length;
int count;
int num_threads;
pthread_mutex_t count_mutex; // Mutex to guard the critical section

void *count1s(void *arg) {
    int thread_id = *((int *)arg);
    int i;
    int chunk_size = length / num_threads;
    int start = thread_id * chunk_size;
    int end = (thread_id == num_threads - 1) ? length : start + chunk_size;
    int local_count = 0;

    for (i = start; i < end; i++) {
        if (array[i] == 1) {
            local_count++;
        }
    }

    // Acquire the mutex before updating the shared count variable
    pthread_mutex_lock(&count_mutex);
    count += local_count;
    // Release the mutex after updating the shared count variable
    pthread_mutex_unlock(&count_mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <array_length> <num_threads>\n", argv[0]);
        return -1;
    }

    length = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    array = (int *)malloc(length * sizeof(int));

    // Initialize array with random values between 0 and 5 (inclusive)
    srand(time(NULL));
    for (int i = 0; i < length; i++) {
        array[i] = rand() % 6;
    }

    // Print the array
    printf("Array: ");
    for (int i = 0; i < length; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");

    pthread_mutex_init(&count_mutex, NULL); // Initialize the mutex

    clock_t start_time = clock();

    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, count1s, (void *)&thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Print the result and timing information
    printf("Count: %d\n", count);
    printf("Elapsed time: %f seconds\n", elapsed_time);

    pthread_mutex_destroy(&count_mutex); // Destroy the mutex
    free(array);

    return 0;
}
