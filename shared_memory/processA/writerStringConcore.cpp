#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

#define SHARED_MEMORY_KEY 99242526
#define BUFFER_SIZE 256

class Writer {
private:
    int shm_id;
    char* sharedData_;

public:
    Writer() {
        // Generate a unique key for the shared memory segment
        shm_id = shmget(SHARED_MEMORY_KEY, BUFFER_SIZE, IPC_CREAT | 0666);
        if (shm_id == -1) {
            std::cerr << "Failed to create shared memory segment." << std::endl;
        }

        // Attach the shared memory segment to the process's address space
        sharedData_ = static_cast<char*>(shmat(shm_id, NULL, 0));
        if (sharedData_ == reinterpret_cast<char*>(-1)) {
            std::cerr << "Failed to attach shared memory segment." << std::endl;
        }
    }

    ~Writer() {
        // Detach the shared memory segment from the process
        shmdt(sharedData_);

        // Remove the shared memory segment
        shmctl(shm_id, IPC_RMID, nullptr);
    }

    void run() {
        // Write data to the shared memory
        std::string value = "";
        std::strncpy(sharedData_, value.c_str(), BUFFER_SIZE - 1);

        int count = 1;
        while (count < 100) {
            while (true) {
                if (std::strcmp(value.c_str(), sharedData_) == 0) {
                    continue;
                } else {
                    std::string message(sharedData_, strnlen(sharedData_, BUFFER_SIZE));
                    // std::cout << message.length() << std::endl; 
                    std::cout << "Received message: " << message << std::endl;
                    value = "nitin";
                    std::strncpy(sharedData_, value.c_str(), BUFFER_SIZE - 1);
                    break;
                }
                sleep(3);
            }
            sleep(1);

            count = count + 1;
        }

        // Signal the end of the loop to the reader
        *sharedData_ = -1;
    }
};


int main() {
    Writer writer;
    writer.run();

    return 0;
}
