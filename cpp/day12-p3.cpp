#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
// #include <atomic> // 기존 코드: 현재 파일에서 사용하지 않음
#include <cassert>
#include <stdexcept>
#include <exception> // BP: std::current_exception()의 공식 헤더를 직접 포함
#include <future>
#include <memory>
#include <queue>
#include <iostream>
#include <algorithm>


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
                // cv_.wait(lock, [&](){ return stopped_ || !tasks_.empty(); }); // 기존 코드
                cv_.wait(lock, [this] { return stopped_ || !tasks_.empty(); }); // BP: 필요한 this만 캡처

                if(stopped_ && tasks_.empty()) { return; }
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
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

    std::future<int> submit(std::function<int()> task) {
        auto promise = std::make_shared<std::promise<int>>();
        auto future = promise->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if(stopped_) { throw std::runtime_error("WorkerPool is stopped"); }

            tasks_.push([promise, task = std::move(task)]() {
                try {
                    promise->set_value(task());
                }
                catch(...) {
                    promise->set_exception(std::current_exception());
                }
            });
        }
        cv_.notify_one();
        return future;
    }

    void shutdown() {
        {
            // std::unique_lock<std::mutex> lock(mutex_); // 기존 코드
            std::lock_guard<std::mutex> lock(mutex_); // BP: wait나 수동 unlock이 없어 lock_guard면 충분
            if(stopped_) { return; }

            stopped_ = true;
        }

        cv_.notify_all();
        // for(auto& worker: workers_) { worker.join(); } // 기존 코드
        for(auto& worker: workers_) {
            if(worker.joinable()) { worker.join(); } // BP: join 가능한 thread만 join
        }
    }
};

std::vector<int> topKLargest(const std::vector<int>& nums, std::size_t k) {
    if (k == 0) {
        return {};
    }


    std::priority_queue<int> queue;

    for (std::size_t i = 0; i < nums.size(); ++i) {
        int target = -nums[i];

        if(queue.size() < k) {
            queue.push(target);
        }
        else if(auto top = queue.top(); top > target) {
            queue.pop();
            queue.push(target);
        }
    }

    std::vector<int> ret;
    while(!queue.empty()) {
        ret.emplace_back(queue.top() * -1);
        queue.pop();
    }

    std::reverse(ret.begin(), ret.end());
    return ret;
}

int main() {
    WorkerPool worker_pool(1);
    auto f1 = worker_pool.submit([] { return 21 + 21; });
    assert(f1.get() == 42);

    // auto f2 = worker_pool.submit([] { // 기존 코드
    //     throw std::runtime_error("error!");
    //     return 0;
    // });
    auto f2 = worker_pool.submit([]() -> int { // BP: 실제 반환 없이 throw하는 함수의 반환형을 명시
        throw std::runtime_error("error!");
    });

    // bool caught = false; // 기존 코드
    bool task_exception_caught = false; // BP: 검증 대상을 이름에 명시
    try {
        f2.get();
    } catch (...) {
        // caught = true; // 기존 코드
        task_exception_caught = true;
    }
    // assert(caught); // 기존 코드
    assert(task_exception_caught);

    auto f3 = worker_pool.submit([] { return 21 + 21; });
    assert(f3.get() == 42);

    worker_pool.shutdown();

    // caught = false; // 기존 코드
    bool shutdown_exception_caught = false; // BP: 앞선 예외 검사와 상태를 분리
    try {
        worker_pool.submit([] { return 21 + 21; });
    } catch (...) {
        // caught = true; // 기존 코드
        shutdown_exception_caught = true;
    }
    // assert(caught); // 기존 코드
    assert(shutdown_exception_caught);

    //워밍업 코드

    assert((topKLargest({3, 2, 1, 5, 6, 4}, 2) == std::vector<int>{6, 5}));
    assert((topKLargest({3, 2, 3, 1, 2, 4, 5, 5, 6}, 4) == std::vector<int>{6, 5, 5, 4}));
    assert(topKLargest({1, 2, 3}, 0).empty());
    assert(topKLargest({}, 3).empty());
    assert((topKLargest({4, 4, 4}, 3) == std::vector<int>{4, 4, 4}));

    return 0;
}
