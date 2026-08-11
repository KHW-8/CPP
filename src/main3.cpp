#include <print>
#include <iostream>
#include <thread>

class Worker {
public:
    void doWork(int count) {
        for (int i = 0; i < count; ++i) {
            std::cout << "Working in thread... " << i << '\n';
        }
    }

    void startThread() {
        // Pass function pointer, instance pointer (this), and arguments
        std::thread t(&Worker::doWork, this, 5);

        // Always join or detach the thread before it goes out of scope
        t.join();
    }
};

int main() {
    Worker worker;
    std::print("Start\n");
    worker.startThread();
    std::print("End\n");
    return 0;
}