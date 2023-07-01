#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>

#define SHARED_MEMORY_KEY 1234
#define BUFFER_SIZE 256

int main() {
    // Get the shared memory segment
    int shm_id = shmget(SHARED_MEMORY_KEY, BUFFER_SIZE, 0666);
    if (shm_id == -1) {
        std::cerr << "Failed to get shared memory segment." << std::endl;
        return 1;
    }

    // Attach the shared memory segment
    char* shared_memory = static_cast<char*>(shmat(shm_id, NULL, 0));
    if (shared_memory == reinterpret_cast<char*>(-1)) {
        std::cerr << "Failed to attach shared memory segment." << std::endl;
        return 1;
    }

    // Wait for new data to be available
    while (shared_memory[BUFFER_SIZE - 1] != '*') {
        usleep(100);
    }

    // Read and print the data from the shared memory
    std::string message(shared_memory, BUFFER_SIZE - 1);
    std::cout << "Received message: " << message << std::endl;

    // Notify the writer that data has been read
    shared_memory[BUFFER_SIZE - 1] = '!';

    // Detach the shared memory segment
    shmdt(shared_memory);

    return 0;
}
