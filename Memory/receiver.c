#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../common/C/benchmark.h"
#include "../common/C/ipc.h"

#define RECEIVE_MODE 1 // process shall listen to the message queue and save the data from shared memory
#define NOTIFY_MODE 2  // process shall notify the other receiver that the last process finished

#define LIST_COUNT 1200

char *filenames[12] = {
    "Memory_Allocation_C_Regular",
    "Memory_Allocation_C_Manual",
    "Memory_Allocation_C_Container",
    "Memory_Allocation_C_Privileged",
    "Memory_Write_C_Regular",
    "Memory_Write_C_Manual",
    "Memory_Write_C_Container",
    "Memory_Write_C_Privileged",
    "Memory_Read_C_Regular",
    "Memory_Read_C_Manual",
    "Memory_Read_C_Container",
    "Memory_Read_C_Privileged",
};

int **setup_arrays()
{
    int **arrays = malloc(sizeof(int *) * LIST_COUNT);
    for (int i = 0; i < LIST_COUNT; i++)
    {
        arrays[i] = (int *)malloc(sizeof(long) * 128);
    }

    return arrays;
}

void print_lists(int **arrays)
{
    for (int i = 0; i < LIST_COUNT; i++)
    {
        printf("Printing list %d:\n", i);
        long *array = (long *)arrays[i];

        for (int j = 0; j < 128; j++)
        {
            printf("(%d, %ld) ", j, (long)array[j]);
        }
        printf("\n");
    }
}

void save_results(int **arrays, char *directory)
{
    char filename[100];
    for (int i = 0; i < LIST_COUNT; i++)
    {
        sprintf(filename, "%s/%s", directory, filenames[i / 100]);
        float *array = (float *)arrays[i];

        for (int j = 0; j < 128; j++)
        {
            float value = (float)array[j];
            if (value == 0)
                continue;

            save_benchmark_result_partial(value, j, filename);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("To few arguments: %d\n", argc - 1);
        return -1;
    }

    // Create shared memory object
    int message_queue_id = 0;
    int shared_memory_id = 0;
    initialize_ipc(MODE_CREATOR, "ipc/memory", &message_queue_id, &shared_memory_id);

    int mode = atoi(argv[1]);
    if (mode == NOTIFY_MODE)
    {
        notify_receiver(message_queue_id);
        return 0;
    }

    if (mode != RECEIVE_MODE)
    {
        printf("Invalid mode: %d\n", mode);
    }

    float *address = get_shared_memory(shared_memory_id);

    int **arrays = setup_arrays();
    int array_index = -1; // -1 so it's 0 after the first notification arrived

    struct message message;

    while (array_index < LIST_COUNT)
    {
        int success = msgrcv(message_queue_id, &message, sizeof(message), 1, 0);
        handle_error(success, "msgrcv()");

        // measurement programs notify the receiver before the start measuring.
        // That means data is only available when the second measurement process send a notification
        // Therefore, skip copying memory unless we received the second notification (we started at -1 and increase after
        // copying, so if array_index is 0, we received the second message)
        if (array_index >= 0)
        {
            // Copy data from shared memory in the lists and then erase shared memory
            float *array = (float *)arrays[array_index];
            memcpy(array, address, sizeof(float) * 128);
            memset(address, 0, sizeof(float) * 128);
        }

        array_index++;
    }

    save_results(arrays, argv[2]);

    msgctl(shared_memory_id, IPC_RMID, NULL);
    msgctl(message_queue_id, IPC_RMID, NULL);

    return 0;
}