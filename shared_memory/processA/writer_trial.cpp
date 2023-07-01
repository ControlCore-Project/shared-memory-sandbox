#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>

class Writer {
private:
    int shmId_;
    int* sharedData_;

public:
    Writer() {
        // Generate a unique key for the shared memory segment
        key_t key = ftok("shared_memory_key", 'R');

        // Create a shared memory segment
        shmId_ = shmget(key, sizeof(int), IPC_CREAT | 0666);

        // Attach the shared memory segment to the process's address space
        sharedData_ = static_cast<int*>(shmat(shmId_, nullptr, 0));
    }

    ~Writer() {
        // Detach the shared memory segment from the process
        shmdt(sharedData_);

        // Remove the shared memory segment
        shmctl(shmId_, IPC_RMID, nullptr);
    }

    void run() {
        // Write data to the shared memory
        int value = 0;
        *sharedData_ = value;

        int count = 1;
        while (count < 100) {
            while (true) {
                if (value == *sharedData_) {
                    continue;
                } else {
                    std::cout << "Shared data received: " << *sharedData_ << std::endl;
                    *sharedData_ += 4;
                    value = *sharedData_;
                    break;
                }
                sleep(10);
            }
            sleep(10);

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