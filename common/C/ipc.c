#ifndef IPC_H
#define IPC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/shm.h>
#include <sys/msg.h>

#include "benchmark.h"
#include "ipc.h"

void initialize_ipc(int mode, int *message_queue_id, int *shared_memory_id)
{
    if (mode != MODE_CREATOR && mode != MODE_USER)
    {
        printf("Wrong mode: %d\n", mode);
        exit(-1);
    }

    key_t shared_memory_key = ftok("memory", 1);
    handle_error(shared_memory_key, "ftok() 1");

    key_t message_queue_key = ftok("memory", 2);
    handle_error(message_queue_key, "ftok() 2");

    int shared_memory_flags = (mode == MODE_CREATOR) ? 0600 | IPC_CREAT : 0;
    int message_queue_flags = (mode == MODE_CREATOR) ? 0666 | IPC_CREAT : 0;

    int sid = shmget(shared_memory_key, SHARED_MEMORY_SIZE, shared_memory_flags);
    handle_error(sid, "shmget()");

    int mid = msgget(message_queue_key, message_queue_flags);
    handle_error(mid, "shmget()");

    *message_queue_id = mid;
    *shared_memory_id = sid;
}

long *get_shared_memory(int shared_memory_id)
{
    long *address = (long *)shmat(shared_memory_id, NULL, 0);
    handle_error((int)*address, "shmat()");

    return address;
}

void notify_receiver(int message_queue_id)
{
    struct message message;
    message.type = 1;
    strcpy(message.text, "0"); // Message text doesn't matter

    int success = msgsnd(message_queue_id, &message, sizeof(message), 0);
    handle_error(success, "msgsnd()");
}

#endif