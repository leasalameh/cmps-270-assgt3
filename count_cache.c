/*
https://github.com/leasalameh/cmps-270-assgt3
username: leasalameh, repo: cmps-270-assgt3

1,000 integers, 1 thread: Elapsed time: 0.001000 seconds
1,000 integers, 2 threads: Elapsed time: 0.000000 seconds
1,000 integers, 4 threads: Elapsed time: 0.001000 seconds
1,000 integers, 32 threads:  Elapsed time: 0.002000 seconds

1,000,000 integers, 1 thread: Elapsed time: 0.003000 seconds
1,000,000 integers, 2 threads: Elapsed time: 0.002000 seconds
1,000,000 integers, 4 threads: Elapsed time: 0.001000 seconds
1,000,000 integers, 32 threads: Elapsed time: 0.002000 seconds

1,000,000,000 integers, 1 thread: Elapsed time: 2.286000 seconds
1,000,000,000 integers, 2 threads: Elapsed time: 1.180000 seconds
1,000,000,000 integers, 4 threads: Elapsed time: 0.712000 seconds
1,000,000,000 integers, 32 threads: Elapsed time: 0.642000 seconds

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define CACHE_LINE_SIZE 64 // The size of L1 cache on my system

struct PrivateCount
{
    int count;
    char padding[CACHE_LINE_SIZE - sizeof(int)]; // Padding to fill the cache line
};

int *array;
int length;
struct PrivateCount *private_counts; // Array of private counts for each thread
int num_threads;

void *count1s(void *arg)
{
    int thread_id = *((int *)arg);
    int i;
    int chunk_size = length / num_threads;
    int start = thread_id * chunk_size;
    int end = (thread_id == num_threads - 1) ? length : start + chunk_size;

    struct PrivateCount *private_count = &private_counts[thread_id];

    for (i = start; i < end; i++)
    {
        if (array[i] == 1)
        {
            private_count->count++;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <array_length> <num_threads>\n", argv[0]);
        return -1;
    }

    length = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    array = (int *)malloc(length * sizeof(int));
    private_counts = (struct PrivateCount *)malloc(num_threads * sizeof(struct PrivateCount));

    // Initialize array with random values between 0 and 5 (inclusive)
    srand(time(NULL));
    for (int i = 0; i < length; i++)
    {
        array[i] = rand() % 6;
    }

    clock_t start_time = clock();

    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    // Create threads
    for (int i = 0; i < num_threads; i++)
    {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, count1s, (void *)&thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Combine the private counts to get the total count
    int total_count = 0;
    for (int i = 0; i < num_threads; i++)
    {
        total_count += private_counts[i].count;
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Print the result and timing information
    printf("Count: %d\n", total_count);
    printf("Elapsed time: %f seconds\n", elapsed_time);

    free(array);
    free(private_counts);

    return 0;
}
