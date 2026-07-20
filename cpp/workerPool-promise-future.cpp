/*
INTERVIEW QUESTION — Worker Pool with promise/future (45분)

고정된 수의 worker thread가 int 결과를 반환하는 task를 실행하는 Worker Pool을 구현하라.

제공할 API

    class WorkerPool {
    public:
        explicit WorkerPool(std::size_t workerCount);
        ~WorkerPool();

        std::future<int> submit(std::function<int()> task);
        void shutdown();
    };

동작 계약

- submit()은 task 결과를 받을 future를 즉시 반환한다.
- task가 반환한 값은 future::get()으로 얻고, task 예외도 future::get()에서 다시 발생한다.
- task는 내부 mutex를 소유하지 않은 상태에서 실행한다.
- 한 task의 예외가 process를 종료하거나 worker를 잃게 하면 안 된다.
- shutdown()은 새 task를 거부하고 이미 받은 task를 모두 실행한 뒤 join한다.
- shutdown()은 여러 번 호출해도 안전하며 destructor도 shutdown을 수행한다.
- workerCount 0, 빈 task, shutdown 이후 submit은 예외로 거부한다.

핵심 질문

1. promise를 shared_ptr로 소유하는 이유는 무엇인가?
   답: queue의 std::function은 copyable callable을 요구하지만 promise는 move-only이므로 wrapper를 copyable하게 만들기 위해서다.
2. task 예외는 어떻게 future에 전달되는가?
   답: wrapper가 current_exception()을 promise::set_exception()에 저장하면 future::get()이 같은 예외를 다시 던진다.
3. task 결과는 어떻게 future에 전달되는가?
   답: submit이 promise에서 future를 먼저 얻고 worker wrapper가 task 반환값을 promise::set_value()에 저장한다.
*/

// ASan/UBSan:
// clang++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     workerPool-promise-future.cpp -o pool-future && ./pool-future
//
// ThreadSanitizer:
// clang++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread workerPool-promise-future.cpp -o pool-future-tsan && ./pool-future-tsan

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

class WorkerPool {
    using Task = std::function<int()>;
    using Work = std::function<void()>;

    std::queue<Work> tasks_;
    std::vector<std::thread> workers_;
    bool closed_ = false;

    std::mutex mutex_;
    std::condition_variable cv_;

    void work() {
        while (true) {
            Work task;
            {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [this] { return closed_ || !tasks_.empty(); });

                if (tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            try {
                task();
            } catch (...) {
                // promise wrapper 밖으로 나온 예외도 worker를 종료시키지 않는다.
            }
        }
    }

public:
    explicit WorkerPool(std::size_t workerCount) {
        if (workerCount == 0) {
            throw std::invalid_argument("workerCount must be positive");
        }

        for (std::size_t i = 0; i < workerCount; ++i) {
            workers_.emplace_back(&WorkerPool::work, this);
        }
    }

    ~WorkerPool() { shutdown(); }

    std::future<int> submit(Task task) {
        if (!task) {
            throw std::invalid_argument("task must not be empty");
        }

        auto promise = std::make_shared<std::promise<int>>();
        auto future = promise->get_future();

        {
            std::lock_guard lock(mutex_);
            if (closed_) {
                throw std::runtime_error("WorkerPool is shut down");
            }

            tasks_.push([promise, task = std::move(task)] {
                try {
                    promise->set_value(task());
                } catch (...) {
                    promise->set_exception(std::current_exception());
                }
            });
        }

        cv_.notify_one();
        return future;
    }

    void shutdown() {
        {
            std::lock_guard lock(mutex_);
            if (closed_) {
                return;
            }
            closed_ = true;
        }

        cv_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
    }
};

void testResultAndDrainShutdown() {
    WorkerPool pool(4);
    std::vector<std::future<int>> futures;

    for (int value = 0; value < 100; ++value) {
        futures.push_back(pool.submit([value] { return value * 2; }));
    }
    pool.shutdown();

    for (int value = 0; value < 100; ++value) {
        assert(futures[value].get() == value * 2);
    }
}

void testExceptionPropagationAndWorkerSurvival() {
    WorkerPool pool(1);
    auto failed = pool.submit([]() -> int {
        throw std::runtime_error("task failed");
    });
    auto succeeded = pool.submit([] { return 42; });

    bool caught = false;
    try {
        failed.get();
    } catch (const std::runtime_error&) {
        caught = true;
    }

    assert(caught);
    assert(succeeded.get() == 42);
}

void testBoundariesAndDestructorShutdown() {
    std::vector<std::future<int>> futures;
    {
        WorkerPool pool(2);
        for (int value = 0; value < 20; ++value) {
            futures.push_back(pool.submit([value] { return value; }));
        }
    }

    for (int value = 0; value < 20; ++value) {
        assert(futures[value].get() == value);
    }

    WorkerPool closed(1);
    closed.shutdown();

    bool rejectedAfterShutdown = false;
    try {
        closed.submit([] { return 1; });
    } catch (const std::runtime_error&) {
        rejectedAfterShutdown = true;
    }
    assert(rejectedAfterShutdown);

    bool rejectedZeroWorkers = false;
    try {
        WorkerPool invalid(0);
    } catch (const std::invalid_argument&) {
        rejectedZeroWorkers = true;
    }
    assert(rejectedZeroWorkers);
}

int main() {
    testResultAndDrainShutdown();
    testExceptionPropagationAndWorkerSurvival();
    testBoundariesAndDestructorShutdown();
}
