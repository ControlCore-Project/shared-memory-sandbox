#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>

class Reader {
private:
    int shmId_;
    int* sharedData_;

public:
    Reader() {
        // Generate the same unique key for the shared memory segment
        key_t key = ftok("shared_memory_key", 'R');

        // Get the shared memory segment created by Writer
        while (true) {
            // Get the shared memory segment created by Writer
            shmId_ = shmget(key, sizeof(int), 0666);

            // Check if shared memory exists
            if (shmId_ != -1) {
                break; // Break the loop if shared memory exists
            }

            std::cout << "Shared memory does not exist. Make sure the writer process is running." << std::endl;
            sleep(1); // Sleep for 1 second before checking again
        }

        // Attach the shared memory segment to the process's address space
        sharedData_ = static_cast<int*>(shmat(shmId_, nullptr, 0));
    }

    ~Reader() {
        // Detach the shared memory segment from the process
        shmdt(sharedData_);
        shmctl(shmId_, IPC_RMID, nullptr);
    }

    void run() {
        int value = -1;

        int count = 0;
        while (count < 100) {
            while (true) {
                if (value == *sharedData_) {
                    continue;
                } else {
                    // Read data from the shared memory
                    std::cout << "Shared data received: " << *sharedData_ << std::endl;
                    *sharedData_ += 10;
                    value = *sharedData_;
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