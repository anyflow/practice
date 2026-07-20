/*
INTERVIEW QUESTION — Synchronous In-memory Pub/Sub (40분)

topic별 callback을 등록하고 메시지를 동기적으로 전달하는 in-memory Pub/Sub을 구현하라.
callback 실행 순서는 보장하지 않는다.

제공할 API

    class PubSub {
    public:
        using Callback = std::function<void(const std::string&)>;
        using SubscriptionId = std::size_t;

        SubscriptionId subscribe(std::string topic, Callback callback);
        bool unsubscribe(const std::string& topic, SubscriptionId id);
        void publish(const std::string& topic, const std::string& message);
    };

동작 계약

- subscribe()는 구독을 식별하는 고유 id를 반환한다.
- unsubscribe()는 실제 구독을 제거했을 때만 true를 반환한다.
- publish()는 호출 시 만든 callback snapshot을 현재 thread에서 실행한다.
- snapshot을 만들기 전에 완료된 unsubscribe는 현재 publish에서 실행되지 않는다.
- snapshot 이후의 subscribe·unsubscribe는 현재 publish의 callback 집합을 바꾸지 않는다.
- callback 실행 중에는 내부 mutex를 소유하면 안 된다.
- 한 callback의 예외가 나머지 callback 실행을 막으면 안 된다.

제출 전 검증

- topic 분리, subscribe와 unsubscribe의 성공·실패
- callback의 재진입 publish와 self-unsubscribe
- callback 예외 격리
- 동시 subscribe와 동시 publish 및 ThreadSanitizer 검사

Follow-up

1. callback을 내부 락 아래에서 실행하면 어떤 문제가 생기는가?
   답: 느린 callback이 모든 연산을 막고 callback의 재진입 subscribe·unsubscribe·publish가 deadlock을 일으킬 수 있다.
2. snapshot 복사에는 어떤 비용이 있는가?
   답: publish마다 subscriber 수에 비례하는 O(n) 시간·메모리와 std::function 복사 비용이 발생한다.
3. unsubscribe 반환 후 실행 중인 callback까지 없음을 보장하려면 무엇이 필요한가?
   답: 구독별 실행 중 횟수를 추적하고 unsubscribe가 0이 될 때까지 기다리되 self-unsubscribe는 별도로 처리해야 한다.
4. 비동기 전달로 바꾸면 queue와 Worker Pool을 어디에 둘 것인가?
   답: publish가 callback·message 작업을 bounded queue에 넣고 별도 Worker Pool이 내부 락 없이 꺼내 실행하게 한다.
5. 느린 subscriber로 queue가 쌓일 때 drop·block·reject를 어떻게 선택하는가?
   답: 유실 허용 이벤트는 drop, 내부 backpressure가 필요하면 block, 호출자에게 재시도를 맡기려면 reject를 선택한다.
6. at-most-once보다 강한 전달 보장이 필요하면 어떤 상태를 영속화해야 하는가?
   답: message와 고유 ID, subscription, subscriber별 전달·ack 상태를 영속화해야 한다.
*/

// ASan/UBSan:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     PubSub.cpp -o pubsub && ./pubsub
//
// ThreadSanitizer:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread PubSub.cpp -o pubsub-tsan && ./pubsub-tsan

#include <functional>
#include <string>
#include <cassert>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <stdexcept>
#include <utility>

class PubSub {
public:
    using SubscriptionId = std::size_t;
    using Callback = std::function<void(const std::string&)>;

private:
    using Callbacks = std::unordered_map<SubscriptionId, Callback>;

    std::unordered_map<std::string, Callbacks> topics_;
    SubscriptionId last_id_ = 0;

    std::mutex mutex_;

public:
    void publish(const std::string& topic, const std::string& message) {
        std::vector<Callback> callbacks;
        {
            std::lock_guard lock(mutex_);

            auto found = topics_.find(topic);
            if(found == topics_.end()) {
                return;
            }

            for(auto& [_, callback]: found->second) {
                callbacks.push_back(callback);
            }
        }

        for(auto& callback: callbacks) {
            try {
                callback(message);
            }
            catch(...) {
                // just for interview...
            }
        }
    }

    SubscriptionId subscribe(std::string topic, Callback callback) {
        std::lock_guard lock(mutex_);

        auto found = topics_.find(topic);
        if(found == topics_.end()) {
            found = topics_.try_emplace(std::move(topic)).first;
        }
        found->second.try_emplace(last_id_, std::move(callback));

        return last_id_++;
    }

    bool unsubscribe(const std::string& topic, SubscriptionId id) {
        std::lock_guard lock(mutex_);

        auto found = topics_.find(topic);
        if(found == topics_.end()) {
            return false;
        }

        auto result = found->second.erase(id) > 0;

        if(found->second.empty()) {
            topics_.erase(found);
        }

        return result;
    }
};

void testBasicBehavior() {
    PubSub pubsub;
    std::vector<std::string> ordersA;
    std::vector<std::string> ordersB;
    std::vector<std::string> payments;

    const auto idA = pubsub.subscribe("orders", [&](const std::string& message) {
        ordersA.push_back(message);
    });
    const auto idB = pubsub.subscribe("orders", [&](const std::string& message) {
        ordersB.push_back(message);
    });
    pubsub.subscribe("payments", [&](const std::string& message) {
        payments.push_back(message);
    });

    assert(idA != idB);
    pubsub.publish("orders", "created");
    assert((ordersA == std::vector<std::string>{"created"}));
    assert((ordersB == std::vector<std::string>{"created"}));
    assert(payments.empty());

    assert(pubsub.unsubscribe("orders", idA));
    assert(!pubsub.unsubscribe("orders", idA));
    pubsub.publish("orders", "shipped");

    assert((ordersA == std::vector<std::string>{"created"}));
    assert((ordersB == std::vector<std::string>{"created", "shipped"}));
}

void testReentrancyAndExceptionIsolation() {
    PubSub pubsub;
    int auditCalls = 0;
    int normalCalls = 0;

    pubsub.subscribe("audit", [&](const std::string& message) {
        assert(message == "created");
        ++auditCalls;
    });
    pubsub.subscribe("orders", [&](const std::string& message) {
        pubsub.publish("audit", message); // 내부 락 밖에서 실행되므로 재진입 가능하다.
    });
    pubsub.publish("orders", "created");
    assert(auditCalls == 1);

    pubsub.subscribe("errors", [](const std::string&) {
        throw std::runtime_error("callback failed");
    });
    pubsub.subscribe("errors", [&](const std::string&) {
        ++normalCalls;
    });
    pubsub.publish("errors", "message");
    assert(normalCalls == 1);

    int selfCalls = 0;
    PubSub::SubscriptionId selfId = 0;
    selfId = pubsub.subscribe("self", [&](const std::string&) {
        ++selfCalls;
        assert(pubsub.unsubscribe("self", selfId));
    });
    pubsub.publish("self", "first");
    pubsub.publish("self", "second");
    assert(selfCalls == 1);
}

void testConcurrentUse() {
    constexpr int threadCount = 8;
    constexpr int publishesPerThread = 100;

    PubSub pubsub;
    std::atomic<int> subscribedCalls = 0;
    std::vector<std::thread> subscribers;

    for (int id = 0; id < threadCount; ++id) {
        subscribers.emplace_back([&] {
            pubsub.subscribe("subscribers", [&](const std::string&) {
                ++subscribedCalls;
            });
        });
    }
    for (auto& thread : subscribers) {
        thread.join();
    }

    pubsub.publish("subscribers", "message");
    assert(subscribedCalls == threadCount);

    std::atomic<int> publishedCalls = 0;
    pubsub.subscribe("publish", [&](const std::string&) {
        ++publishedCalls;
    });

    std::vector<std::thread> publishers;
    for (int id = 0; id < threadCount; ++id) {
        publishers.emplace_back([&] {
            for (int i = 0; i < publishesPerThread; ++i) {
                pubsub.publish("publish", "message");
            }
        });
    }
    for (auto& thread : publishers) {
        thread.join();
    }

    assert(publishedCalls == threadCount * publishesPerThread);
}

int main() {
    testBasicBehavior();
    testReentrancyAndExceptionIsolation();
    testConcurrentUse();
}
