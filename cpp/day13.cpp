#include <functional>
#include <string>
#include <utility>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <cassert>
#include <vector>
#include <atomic>
#include <iostream>
#include <stdexcept>
#include <queue>
#include <unordered_set>

class PubSub {
public:
    using Callback = std::function<void(const std::string&)>;
    using Subscribers = std::unordered_map<std::size_t, Callback>;

private:
    std::size_t last_id_ = 0;
    std::unordered_map<std::string, Subscribers> topics_;
    std::mutex mutex_;

  public:
    std::size_t subscribe(std::string topic, Callback callback) {
        std::lock_guard<std::mutex> lock(mutex_);

        ++last_id_;

        auto found = topics_.try_emplace(std::move(topic)).first;
        found->second.emplace(last_id_, std::move(callback));

        return last_id_;
    }

    bool unsubscribe(const std::string& topic, std::size_t id) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto found = topics_.find(topic);
        if(found == topics_.end()) { return false; }

        auto found_id = found->second.find(id);
        if(found_id == found->second.end()) { return false; }

        found->second.erase(found_id);
        return true;
    }

    void publish(const std::string& topic, const std::string& message) {
        std::vector<Callback> cbs;
        {
            std::lock_guard<std::mutex> lock(mutex_);

            auto found = topics_.find(topic);
            if(found == topics_.end()) { return; }

            for(const auto& subscriber: found->second) {
                cbs.push_back(subscriber.second);
            }
        }

        for(const auto& cb: cbs) {
            try{
                cb(message);
            }
            catch(...) {
                std::cerr << "unknown exception\n";
            }
        }
    }
};

void p1() {
    PubSub pubsub;

    // 1. subscribe는 서로 다른 id를 반환하고 topic별 callback을 분리한다.
    std::vector<std::string> receivedA;
    std::vector<std::string> receivedB;
    std::vector<std::string> receivedC;

    auto idA = pubsub.subscribe("orders", [&](const std::string& str) { receivedA.push_back(str); });
    assert(idA);
    auto idB = pubsub.subscribe("orders", [&](const std::string& str) { receivedB.push_back(str); });
    assert(idB);
    assert(idA != idB);
    auto idC = pubsub.subscribe("payments", [&](const std::string& str) { receivedC.push_back(str); });
    assert(idC != idA && idC != idB);

    // 2. publish는 해당 topic의 subscriber에게만 메시지를 전달한다.
    pubsub.publish("orders", "created");
    assert((receivedA == std::vector<std::string>{"created"}));
    assert((receivedB == std::vector<std::string>{"created"}));
    assert(receivedC.empty());

    pubsub.publish("payments", "paid");
    assert((receivedC == std::vector<std::string>{"paid"}));

    // 3. unsubscribe 이후에는 해당 callback이 호출되지 않으며 재해제는 실패한다.
    assert(pubsub.unsubscribe("orders", idA));

    pubsub.publish("orders", "shipped");
    assert((receivedA == std::vector<std::string>{"created"}));
    assert((receivedB == std::vector<std::string>{"created", "shipped"}));

    assert(!pubsub.unsubscribe("orders", idA));

    // 4. subscriber가 없는 topic의 publish는 아무 상태도 변경하지 않는다.
    pubsub.publish("unknown", "message");
    assert((receivedA == std::vector<std::string>{"created"}));
    assert((receivedB == std::vector<std::string>{"created", "shipped"}));
    assert((receivedC == std::vector<std::string>{"paid"}));

    // 5. 여러 thread가 동시에 등록한 callback이 유실되지 않는다.
    constexpr int thread_count = 8;
    std::atomic<int> subscribed_calls = 0;
    std::vector<std::thread> subscribers;
    for(int i = 0; i < thread_count; ++i) {
        subscribers.emplace_back([&] {
            pubsub.subscribe("concurrent-subscribe", [&](const std::string&) {
                ++subscribed_calls;
            });
        });
    }
    for(auto& thread : subscribers) { thread.join(); }

    pubsub.publish("concurrent-subscribe", "message");
    assert(subscribed_calls == thread_count);

    // 6. 여러 thread가 동시에 publish해도 callback 호출 횟수가 정확하다.
    constexpr int publishes_per_thread = 100;
    std::atomic<int> published_calls = 0;
    pubsub.subscribe("concurrent-publish", [&](const std::string&) {
        ++published_calls;
    });

    std::vector<std::thread> publishers;
    for(int i = 0; i < thread_count; ++i) {
        publishers.emplace_back([&] {
            for(int j = 0; j < publishes_per_thread; ++j) {
                pubsub.publish("concurrent-publish", "message");
            }
        });
    }
    for(auto& thread : publishers) { thread.join(); }

    assert(published_calls == thread_count * publishes_per_thread);
}

void p2() {
    PubSub pubsub;

    // 1. callback에서 다른 topic을 publish해도 교착 없이 두 callback이 실행된다.
    int orders_calls = 0;
    int audit_calls = 0;
    pubsub.subscribe("audit", [&](const std::string& message) {
        assert(message == "created");
        ++audit_calls;
    });
    pubsub.subscribe("orders", [&](const std::string& message) {
        ++orders_calls;
        pubsub.publish("audit", message);
    });
    pubsub.publish("orders", "created");
    assert(orders_calls == 1);
    assert(audit_calls == 1);

    // 2. 한 callback이 예외를 던져도 다른 정상 callback은 실행된다.
    int normal_calls = 0;
    pubsub.subscribe("errors", [](const std::string&) {
        throw std::runtime_error("callback failed");
    });
    pubsub.subscribe("errors", [&](const std::string&) {
        ++normal_calls;
    });
    pubsub.publish("errors", "message");
    assert(normal_calls == 1);

    // 3. callback이 자신을 unsubscribe하면 다음 publish부터 호출되지 않는다.
    int self_calls = 0;
    std::size_t self_id = 0;
    self_id = pubsub.subscribe("self", [&](const std::string&) {
        ++self_calls;
        assert(pubsub.unsubscribe("self", self_id));
    });
    pubsub.publish("self", "first");
    pubsub.publish("self", "second");
    assert(self_calls == 1);

    // 4. 예외 callback 실행 뒤에도 subscribe, publish, unsubscribe가 정상 동작한다.
    int after_error_calls = 0;
    auto after_error_id = pubsub.subscribe("after-error", [&](const std::string&) {
        ++after_error_calls;
    });
    pubsub.publish("after-error", "first");
    assert(pubsub.unsubscribe("after-error", after_error_id));
    pubsub.publish("after-error", "second");
    assert(after_error_calls == 1);

    // 5. P1의 동시성 검증과 함께 ThreadSanitizer로 data race가 없는지 확인한다.
}

using Edges = std::vector<std::pair<std::size_t, std::size_t>>;
using Graph = std::vector<std::unordered_set<std::size_t>>;

// using ComponentCounter = std::size_t (*)(std::size_t, const Edges&);
using ComponentCounter = std::function<std::size_t(std::size_t, const Edges&)>;

void warmingup(ComponentCounter count) {
    // 일반 그래프, 고립 정점, cycle, self-loop와 빈 그래프를 검증한다.
    assert(count(5, Edges{{0, 1}, {1, 2}, {3, 4}}) == 2);
    assert(count(5, Edges{}) == 5);
    assert(count(4, Edges{{0, 1}, {1, 2}, {2, 0}}) == 2);
    assert(count(1, Edges{{0, 0}}) == 1);
    assert(count(0, Edges{}) == 0);
}

std::size_t countConnectedComponents(std::size_t node_count, const Edges& edges) {
    Graph graph(node_count);
    for (const auto& edge : edges) {
        graph[edge.first].insert(edge.second);
        graph[edge.second].insert(edge.first);
    }

    std::size_t components = 0;
    std::vector<bool> visited(node_count, false);
    std::queue<std::size_t> queue;

    for (std::size_t start = 0; start < node_count; ++start) {
        if(visited[start]) {
            continue;
        }

        ++components;
        visited[start] = true;
        queue.push(start);

        while(!queue.empty()) {
            auto curr = queue.front();
            queue.pop();

            for(auto next: graph[curr]) {
                if(visited[next]) {
                   continue;
                }

                visited[next] = true;
                queue.push(next);
            }
        }
    }

    return components;
}

int main() {

    p1();
    p2();

    warmingup(countConnectedComponents);

    return 0;
}
