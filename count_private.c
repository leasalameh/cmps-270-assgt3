#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int *array;
int length;
int *private_counts; // Array of private counts for each thread
int num_threads;

void *count1s(void *arg)
{
    int thread_id = *((int *)arg);
    int i;
    int chunk_size = length / num_threads;
    int start = thread_id * chunk_size;
    int end = (thread_id == num_threads - 1) ? length : start + chunk_size;

    int local_count = 0;

    for (i = start; i < end; i++)
    {
        if (array[i] == 1)
        {
            local_count++;
        }
    }

    // Store the local count in the private_counts array
    private_counts[thread_id] = local_count;

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
    private_counts = (int *)malloc(num_threads * sizeof(int));

    // Initialize array with random values between 0 and 5 (inclusive)
    srand(time(NULL));
    for (int i = 0; i < length; i++)
    {
        array[i] = rand() % 6;
    }

    // Print the array
    printf("Array: ");
    for (int i = 0; i < length; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");

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
        total_count += private_counts[i];
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