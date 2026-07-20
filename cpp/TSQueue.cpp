/*
INTERVIEW QUESTION — Thread-safe Blocking Queue (40분)

여러 producer와 consumer가 공유하는 FIFO queue를 구현하라.

제공할 API

    template <typename T>
    class TSQueue {
    public:
        bool push(T value);
        std::optional<T> waitPop();
        void shutdown();
    };

동작 계약

- waitPop()은 원소가 생기거나 queue가 종료될 때까지 대기한다.
- shutdown 전에 들어온 원소는 FIFO로 모두 꺼낸 뒤 nullopt를 반환한다.
- shutdown 이후 push()는 false를 반환한다.
- shutdown()은 여러 번 호출해도 안전하다.
- 여러 producer와 consumer가 동시에 호출할 수 있다.

제출 전 검증

- 단일 thread의 FIFO와 drain shutdown
- 빈 queue에서 기다리는 consumer의 종료
- shutdown 이후 push 거부와 shutdown의 멱등성
- sleep 없이 다중 producer·consumer의 누락·중복 검사
- ThreadSanitizer 실행

Follow-up

1. condition_variable의 predicate가 필요한 이유는 무엇인가?
   답: spurious wakeup과 상태 변경 경쟁에도 queue가 비었고 열려 있으면 다시 기다리도록 조건을 락 아래 재검사한다.
2. notify는 락을 해제하기 전과 후 중 언제 호출하는 것이 좋은가?
   답: 상태를 락 아래 변경한 뒤 락을 해제하고 notify하면 깨어난 thread가 같은 mutex에서 다시 막히는 일을 줄일 수 있다.
3. drain 대신 남은 원소를 버리는 shutdown은 어떻게 달라지는가?
   답: shutdown이 락 아래 closed를 설정하고 queue를 비운 뒤 모든 waiter를 깨우면 waitPop은 즉시 nullopt를 반환한다.
4. bounded queue를 추가하면 producer의 block·reject 계약은 어떻게 되는가?
   답: 가득 찼을 때 block 방식은 빈자리가 생기거나 종료될 때까지 기다리고 reject 방식은 즉시 false를 반환한다.
5. tryPop()과 timeout을 추가하면 API를 어떻게 설계할 것인가?
   답: tryPop은 즉시 optional을 반환하고 timed wait은 value·timeout·closed를 구분하는 결과 상태를 반환하게 설계한다.
*/

// ASan/UBSan:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     TSQueue.cpp -o tsqueue && ./tsqueue
//
// ThreadSanitizer:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread TSQueue.cpp -o tsqueue-tsan && ./tsqueue-tsan

#include <optional>
#include <cassert>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <barrier>
#include <utility>
#include <algorithm>
#include <condition_variable>

template<typename T>
class TSQueue {
    std::queue<T> queue_;
    bool closed_ = false;

    std::mutex mutex_;
    std::condition_variable cv_;

  public:
    bool push(T value) {
        {
            std::lock_guard lock(mutex_);

            if(closed_) {
                return false;
            }
            queue_.push(std::move(value));
        }

        cv_.notify_one();
        return true;
    }

    std::optional<T> waitPop() {
        std::unique_lock lock(mutex_);
        cv_.wait(lock, [&] { return closed_ || !queue_.empty(); });

        if(!queue_.empty()) {
            auto result = std::move(queue_.front());
            queue_.pop();
            return result;
        }

        return std::nullopt;
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
    }
};

void testFifoAndDrainShutdown() {
    TSQueue<int> queue;

    assert(queue.push(1));
    assert(queue.push(2));
    queue.shutdown();

    assert(queue.waitPop() == 1);
    assert(queue.waitPop() == 2);
    assert(!queue.waitPop());
    assert(!queue.push(3));

    queue.shutdown(); // 멱등성
}

void testWaitingConsumerShutdown() {
    TSQueue<int> queue;
    std::optional<int> result = 0;
    std::barrier startLine(2);

    std::thread consumer([&] {
        startLine.arrive_and_wait();
        result = queue.waitPop();
    });

    startLine.arrive_and_wait();
    queue.shutdown();
    consumer.join();

    assert(!result);
}

void testConcurrentProducersAndConsumers() {
    constexpr int producerCount = 4;
    constexpr int consumerCount = 3;
    constexpr int valuesPerProducer = 250;
    constexpr int totalValues = producerCount * valuesPerProducer;

    TSQueue<int> queue;
    std::barrier startLine(producerCount + consumerCount);
    std::vector<std::vector<int>> consumed(consumerCount);
    std::vector<std::thread> consumers;
    std::vector<std::thread> producers;

    for (int id = 0; id < consumerCount; ++id) {
        consumers.emplace_back([&, id] {
            startLine.arrive_and_wait();
            while (auto value = queue.waitPop()) {
                consumed[id].push_back(*value);
            }
        });
    }

    for (int id = 0; id < producerCount; ++id) {
        producers.emplace_back([&, id] {
            startLine.arrive_and_wait();
            const int begin = id * valuesPerProducer;
            for (int value = begin; value < begin + valuesPerProducer; ++value) {
                assert(queue.push(value));
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }
    queue.shutdown();
    for (auto& consumer : consumers) {
        consumer.join();
    }

    std::vector<int> seen(totalValues, 0);
    for (const auto& values : consumed) {
        for (const int value : values) {
            assert(value >= 0 && value < totalValues);
            ++seen[value];
        }
    }
    assert(std::ranges::all_of(seen, [](int count) { return count == 1; }));
}

int main() {
    testFifoAndDrainShutdown();
    testWaitingConsumerShutdown();
    testConcurrentProducersAndConsumers();
}
