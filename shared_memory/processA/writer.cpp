#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHARED_MEMORY_KEY 1234
#define BUFFER_SIZE 256

int main() {
    // Create the shared memory segment
    int shm_id = shmget(SHARED_MEMORY_KEY, BUFFER_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        std::cerr << "Failed to create shared memory segment." << std::endl;
        return 1;
    }

    // Attach the shared memory segment
    char* shared_memory = static_cast<char*>(shmat(shm_id, NULL, 0));
    if (shared_memory == reinterpret_cast<char*>(-1)) {
        std::cerr << "Failed to attach shared memory segment." << std::endl;
        return 1;
    }

    // Write data to the shared memory
    std::string message = "Hello, reader!";
    std::strncpy(shared_memory, message.c_str(), BUFFER_SIZE - 1);

    // Notify the reader that new data is available
    shared_memory[BUFFER_SIZE - 1] = '*';

    // Wait for the reader to finish reading
    while (shared_memory[BUFFER_SIZE - 1] != '!') {
        usleep(100);
    }

    // Detach the shared memory segment
    shmdt(shared_memory);

    // Delete the shared memory segment
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
