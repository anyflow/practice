#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

class WorkerPool {
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stopped_ = false;
    std::vector<std::thread> workers_;

    void work() {
        while(true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [&](){ return stopped_ || !tasks_.empty(); });

                if(stopped_ && tasks_.empty()) { return; }
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            try {
                task();
            }
            catch(const std::exception& e) {
                std::cerr << "error: " << e.what() << '\n';
            }
            catch(...) {
                std::cerr << "error: unknown exception" << '\n';
            }
        }
    }

public:
    explicit WorkerPool(std::size_t workers) {
        for(std::size_t i=0; i<workers; ++i) {
            workers_.emplace_back([this]{ work(); });
        }
    }
    ~WorkerPool() {
        shutdown();
        for(auto& worker: workers_) { worker.join(); }
    }

    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;
    WorkerPool(WorkerPool&&) = delete;
    WorkerPool& operator=(WorkerPool&&) = delete;

    bool submit(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(stopped_) { return false; }

            tasks_.push(std::move(task));
        }

        cv_.notify_one();
        return true;
    }

    void shutdown() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(stopped_) { return; }

            stopped_ = true;
        }

        cv_.notify_all();
    }
};

void p1() {
    WorkerPool worker_pool(5);
}

int main() {

    p1();

    return 0;
}