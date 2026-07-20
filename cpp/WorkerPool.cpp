/*
INTERVIEW QUESTION — Drain-shutdown Worker Pool (45분)

고정된 수의 worker thread가 제출된 task를 실행하는 Worker Pool을 구현하라.

제공할 API

    class WorkerPool {
    public:
        explicit WorkerPool(std::size_t workerCount);
        ~WorkerPool();

        bool submit(std::function<void()> task);
        void shutdown();
    };

동작 계약

- workerCount는 1 이상이어야 한다.
- submit()은 task를 queue에 넣고, shutdown 이후에는 false를 반환한다.
- task는 queue에서 꺼낸 뒤 내부 mutex를 소유하지 않은 상태에서 실행한다.
- 한 task의 예외가 process를 종료하거나 worker를 잃게 하면 안 된다.
- shutdown()은 새 task를 거부하고 이미 받은 task를 모두 실행한 뒤 join한다.
- shutdown()은 여러 번 호출해도 안전하며 destructor도 shutdown을 수행한다.
- task의 실행 완료 순서는 보장하지 않는다.
- shutdown은 Worker Pool을 소유한 외부 thread에서 호출한다고 가정한다.

제출 전 검증

- 제출된 모든 task가 shutdown 전에 drain되는지 확인
- shutdown 이후 submit 거부와 shutdown의 멱등성
- 예외를 던진 task 다음의 정상 task가 실행되는지 확인
- 여러 producer의 동시 submit에서 누락이 없는지 확인
- destructor의 자동 shutdown과 ThreadSanitizer 실행

Follow-up

1. condition_variable의 predicate와 worker 종료 조건은 무엇인가?
   답: predicate는 closed 또는 task 존재이며, worker는 closed이면서 queue가 비었을 때 종료한다.
2. task를 내부 락 밖에서 실행해야 하는 이유는 무엇인가?
   답: 긴 task가 submit과 다른 worker를 막거나 재진입 시 같은 mutex에서 deadlock되는 것을 방지하기 위해서다.
3. 결과와 예외를 호출자에게 반환하려면 promise/future를 어떻게 연결하는가?
   답: submit에서 promise의 future를 반환하고 queue에는 task 결과나 current_exception을 promise에 저장하는 wrapper를 넣는다.
4. bounded queue에서 block·reject 정책은 어떻게 구현하는가?
   답: full이면 block은 빈자리 또는 종료까지 별도 condition_variable로 기다리고 reject는 즉시 false를 반환한다.
5. 즉시 종료와 drain 종료를 모두 지원하면 상태 전이가 어떻게 달라지는가?
   답: drain은 새 task만 거부하고 queue를 소진한 뒤 종료하며 즉시 종료는 새 task를 거부하고 대기 task도 폐기한다.
6. worker 수는 CPU-bound와 I/O-bound 작업에서 어떻게 정하는가?
   답: CPU-bound는 보통 core 수 근처로, I/O-bound는 대기 비율만큼 더 크게 두되 처리량·메모리·외부 의존성을 측정해 정한다.
*/

// ASan/UBSan:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     WorkerPool.cpp -o pool && ./pool
//
// ThreadSanitizer:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread WorkerPool.cpp -o pool-tsan && ./pool-tsan

#include <cassert>
#include <functional>
#include <future>
#include <atomic>
#include <thread>
#include <barrier>
#include <vector>
#include <utility>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <cstddef>


class WorkerPool {
    std::queue<std::function<void()>> tasks_;
    std::vector<std::thread> workers_;
    bool closed_ = false;

    std::mutex mutex_;
    std::condition_variable cv_;

    void work() {
        while(true) {
            std::function<void()> task;

            {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [&]() { return closed_ || !tasks_.empty(); });

                if(tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            try {
                task();
            } catch (...) {
                // just for the interview...
            }
        }
    }

public:
    explicit WorkerPool(std::size_t workerCount) {
        if(workerCount < 1) {
            throw std::invalid_argument("workerCount must be positive");
        }

        for (std::size_t i = 0; i < workerCount; ++i) {
            workers_.emplace_back([this] { work(); });
        }
    }

    ~WorkerPool() { shutdown(); }

    bool submit(std::function<void()> task) {
        if(!task) {
            return false;
        }

        {
            std::lock_guard lock(mutex_);

            if(closed_) {
                return false;
            }

            tasks_.push(std::move(task));
        }

        cv_.notify_one();
        return true;
    }

    void shutdown() {
        {
            std::lock_guard lock(mutex_);
            if(closed_) {
                return;
            }

            closed_ = true;
        }
        cv_.notify_all();

        for(auto& worker: workers_) {
            worker.join();
        }

    }
};

void testDrainAndShutdown() {
    constexpr int taskCount = 1'000;

    WorkerPool pool(4);
    std::atomic<int> executed = 0;

    for (int i = 0; i < taskCount; ++i) {
        assert(pool.submit([&] { ++executed; }));
    }

    pool.shutdown();

    assert(executed == taskCount);
    assert(!pool.submit([] {}));
    pool.shutdown(); // 멱등성
}

void testTaskExceptionIsolation() {
    WorkerPool pool(1);
    std::promise<void> completed;
    auto future = completed.get_future();

    assert(pool.submit([] {
        throw std::runtime_error("task failed");
    }));
    assert(pool.submit([&] {
        completed.set_value();
    }));

    future.get(); // 예외 task 이후의 정상 task가 실행될 때까지 기다린다.
    pool.shutdown();
}

void testConcurrentSubmit() {
    constexpr int producerCount = 4;
    constexpr int tasksPerProducer = 250;
    constexpr int totalTasks = producerCount * tasksPerProducer;

    WorkerPool pool(4);
    std::barrier startLine(producerCount);
    std::atomic<int> executed = 0;
    std::vector<std::thread> producers;

    for (int id = 0; id < producerCount; ++id) {
        producers.emplace_back([&] {
            startLine.arrive_and_wait();
            for (int i = 0; i < tasksPerProducer; ++i) {
                assert(pool.submit([&] { ++executed; }));
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }
    pool.shutdown();

    assert(executed == totalTasks);
}

void testDestructorShutdownAndBoundaries() {
    std::atomic<int> executed = 0;
    {
        WorkerPool pool(2);
        for (int i = 0; i < 100; ++i) {
            assert(pool.submit([&] { ++executed; }));
        }
    }
    assert(executed == 100);

    bool rejectedZeroWorkers = false;
    try {
        WorkerPool invalid(0);
    } catch (const std::invalid_argument&) {
        rejectedZeroWorkers = true;
    }
    assert(rejectedZeroWorkers);
}

int main() {
    testDrainAndShutdown();
    testTaskExceptionIsolation();
    testConcurrentSubmit();
    testDestructorShutdownAndBoundaries();
}
