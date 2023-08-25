#define MODE_CREATOR 1
#define MODE_USER 2
#define SHARED_MEMORY_SIZE 4096

struct message
{
    long type;
    char text[20];
};

void initialize_ipc(int mode, int *message_queue_id, int *shared_memory_id);
long *get_shared_memory(int shared_memory_id);
void notify_receiver(int message_queue_id);