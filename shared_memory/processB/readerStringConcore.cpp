#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

#define SHARED_MEMORY_KEY 9262626
#define BUFFER_SIZE 256

class Reader {
private:
    int shmId_;
    char* sharedData_;

public:
    Reader() {
        // Get the shared memory segment created by Writer
        while (true) {
            // Get the shared memory segment created by Writer
            shmId_ = shmget(SHARED_MEMORY_KEY, BUFFER_SIZE, 0666);

            // Check if shared memory exists
            if (shmId_ != -1) {
                break; // Break the loop if shared memory exists
            }

            std::cout << "Shared memory does not exist. Make sure the writer process is running." << std::endl;
            sleep(1); // Sleep for 1 second before checking again
        }

        // Attach the shared memory segment to the process's address space
        sharedData_ = static_cast<char*>(shmat(shmId_, NULL, 0));
    }

    ~Reader() {
        // Detach the shared memory segment from the process
        shmdt(sharedData_);
        shmctl(shmId_, IPC_RMID, nullptr);
    }

    void run() {
        std::string value = ".";

        int count = 0;
        while (count < 100) {
            while (true) {
                if (std::strcmp(value.c_str(), sharedData_) == 0) {
                    continue;
                } else {
                    // Read data from the shared memory
                    std::string message(sharedData_, BUFFER_SIZE - 1);
                    std::cout << "Received message: " << message << std::endl;
                    value = "shivang";
                    std::strncpy(sharedData_, value.c_str(), BUFFER_SIZE - 1);
                    break;
                }
                sleep(10);
            }
            sleep(0.2);

            count = count + 1;
        }
    }
};

int main() {
    Reader reader;
    reader.run();

    return 0;
}