/*
INTERVIEW QUESTION — Sharded Thread-safe KV Store (40분)

한 프로세스 안에서 여러 thread가 공유하는 in-memory key-value store를 구현하라.

제공할 API

    template <typename K, typename V, std::size_t ShardCount = 16>
    class KVStore {
    public:
        std::optional<V> get(const K& key) const;
        void put(K key, V value);
        bool erase(const K& key);
        std::size_t size() const;
    };

동작 계약

- get()은 현재 값을 복사해 반환하고, key가 없으면 nullopt를 반환한다.
- put()은 새 key를 삽입하거나 기존 value를 교체한다.
- erase()는 실제로 삭제했을 때만 true를 반환한다.
- 각 호출은 중간 상태가 보이지 않는 하나의 연산이어야 한다.
- 서로 다른 reader는 동시에 실행할 수 있어야 한다.
- key의 hash로 shard를 선택하며, 서로 다른 shard의 연산은 동시에 실행할 수 있어야 한다.
- size()는 모든 shard의 일관된 snapshot을 반환한다.
- 평균 조회·삽입·삭제 시간은 O(1), 저장 공간은 O(n)을 목표로 한다.

제출 전 검증

- put의 삽입·교체, get의 hit·miss, erase의 성공·실패
- 여러 writer가 서로 다른 key를 빠짐없이 저장하는지 확인
- reader와 writer의 동시 실행 및 ThreadSanitizer 검사

Follow-up

1. 내부 value의 reference나 pointer를 반환하면 무엇이 위험한가?
2. shared_mutex가 일반 mutex보다 항상 빠르지 않은 이유는 무엇인가?
3. writer starvation은 어떻게 발생할 수 있는가?
4. ShardCount와 hot shard가 경합·메모리에 주는 영향은 무엇인가?
5. size()나 여러 key transaction이 모든 shard의 lock을 필요로 하는 이유는 무엇인가?
*/

#include <optional>
#include <array>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <cassert>
#include <vector>
#include <thread>
#include <barrier>
#include <utility>

// ASan/UBSan:
// g++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     KVStore-shard.cpp -o kvstore-shard && ./kvstore-shard
//
// ThreadSanitizer:
// g++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread KVStore-shard.cpp -o kvstore-shard-tsan && ./kvstore-shard-tsan

template<typename K, typename V, std::size_t ShardCount = 16>
class KVStore {
    static_assert(ShardCount > 0);

    struct Shard {
        std::unordered_map<K, V> data;
        mutable std::shared_mutex mutex;
    };

    std::array<Shard, ShardCount> shards_;

    static std::size_t shardIndex(const K& key) {
        return std::hash<K>{}(key) % ShardCount;
    }

public:
    KVStore() = default;
    KVStore(const KVStore&) = delete;
    KVStore& operator=(const KVStore&) = delete;

    std::optional<V> get(const K& key) const {
        const Shard& shard = shards_[shardIndex(key)];
        std::shared_lock lock(shard.mutex);

        auto found = shard.data.find(key);
        if(found == shard.data.end()) {
            return std::nullopt;
        }

        return found->second;
    }

    void put(K key, V value) {
        Shard& shard = shards_[shardIndex(key)];
        std::lock_guard lock(shard.mutex);

        shard.data.insert_or_assign(std::move(key), std::move(value));
    }

    bool erase(const K& key) {
        Shard& shard = shards_[shardIndex(key)];
        std::lock_guard lock(shard.mutex);

        return shard.data.erase(key) > 0;
    }

    std::size_t size() const {
        // 모든 shard의 일관된 snapshot을 위해 lock을 모두 유지한 채 합산한다.
        std::vector<std::shared_lock<std::shared_mutex>> locks;
        locks.reserve(ShardCount);
        for (const Shard& shard : shards_) {
            locks.emplace_back(shard.mutex);
        }

        std::size_t total = 0;
        for (const Shard& shard : shards_) {
            total += shard.data.size();
        }
        return total;
    }
};

void testBasicBehavior() {
    KVStore<int, int> store;

    assert(!store.get(1));

    store.put(1, 10);
    assert(store.get(1) == 10);

    store.put(1, 20);
    assert(store.get(1) == 20);
    assert(store.size() == 1);

    assert(store.erase(1));
    assert(!store.erase(1));
    assert(!store.get(1));
}

void testConcurrentReadersAndWriters() {
    constexpr int writerCount = 2;
    constexpr int readerCount = 8;
    constexpr int valuesPerWriter = 1'000;
    constexpr int totalValues = writerCount * valuesPerWriter;

    KVStore<int, int> store;
    std::barrier startLine(writerCount + readerCount);
    std::vector<std::thread> threads;

    for (int writer = 0; writer < writerCount; ++writer) {
        threads.emplace_back([&, writer] {
            startLine.arrive_and_wait();
            const int begin = writer * valuesPerWriter;
            for (int key = begin; key < begin + valuesPerWriter; ++key) {
                store.put(key, key * 10);
            }
        });
    }

    for (int reader = 0; reader < readerCount; ++reader) {
        threads.emplace_back([&] {
            startLine.arrive_and_wait();
            for (int key = 0; key < totalValues; ++key) {
                if (const auto value = store.get(key)) {
                    assert(*value == key * 10);
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    assert(store.size() == totalValues);
    for (int key = 0; key < totalValues; ++key) {
        assert(store.get(key) == key * 10);
    }
}

int main() {
    testBasicBehavior();
    testConcurrentReadersAndWriters();
}
