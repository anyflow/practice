#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>
#include <stdexcept>


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
    }

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
        for(auto& worker: workers_) { worker.join(); }
    }
};

void p1() {
    std::atomic<int> completed{0};

    WorkerPool worker_pool(4);

    for(int i=0; i<1'000; ++i) {
        assert(worker_pool.submit([&completed] {
            completed.fetch_add(1);
        }));
    }

    worker_pool.shutdown();
    worker_pool.shutdown();

    assert(completed.load() == 1'000);
}

void p2() {
    std::atomic<int> completed{0};

    WorkerPool worker_pool(1);

    worker_pool.submit([]{ throw std::runtime_error("error"); });

    for(std::size_t i=0; i<100; ++i) {
        worker_pool.submit([&completed] { completed.fetch_add(1); });
    }

    worker_pool.shutdown();

    assert(completed.load() == 100);
    assert(!worker_pool.submit([&]{ completed.fetch_add(1); }));
}

int main() {

    p1();
    p2();

    return 0;
}
