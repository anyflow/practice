/*
INTERVIEW QUESTION — Thread-safe KV Store (35분)

한 프로세스 안에서 여러 thread가 공유하는 in-memory key-value store를 구현하라.

제공할 API

    template <typename K, typename V>
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
- 평균 조회·삽입·삭제 시간은 O(1), 저장 공간은 O(n)을 목표로 한다.

제출 전 검증

- put의 삽입·교체, get의 hit·miss, erase의 성공·실패
- 여러 writer가 서로 다른 key를 빠짐없이 저장하는지 확인
- reader와 writer의 동시 실행 및 ThreadSanitizer 검사

Follow-up

1. 내부 value의 reference나 pointer를 반환하면 무엇이 위험한가?
   답: lock 해제 후 다른 thread의 삭제·rehash·갱신으로 dangling reference나 data race가 생길 수 있다.
2. shared_mutex가 일반 mutex보다 항상 빠르지 않은 이유는 무엇인가?
   답: mutex는 단일 owner의 잠김·대기 상태를 관리하지만, shared_mutex는 reader 수와 shared/exclusive 대기 상태까지 관리하여 비용이 더 크다.
3. writer starvation은 어떻게 발생할 수 있는가?
   답: shared lock을 잡는 reader가 계속 유입되고 구현이 reader를 우선하면 writer가 exclusive lock을 계속 얻지 못한다.
4. key가 매우 많을 때 lock sharding을 어떻게 적용할 것인가?
   답: `hash(key) % ShardCount`로 key를 shard별 map·mutex에 배정하고 단일 key 연산은 담당 shard만 잠근다.
5. compare-and-set이나 여러 key transaction을 추가하면 무엇이 달라지는가?
   답: 비교와 갱신을 하나의 lock 범위에 묶고, 여러 key는 관련 shard lock을 고정된 순서로 잡아 전체를 원자적으로 처리해야 한다.
*/

#include <optional>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <cassert>
#include <vector>
#include <thread>
#include <barrier>
#include <utility>

// ASan/UBSan:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     KVStore.cpp -o kvstore && ./kvstore
//
// ThreadSanitizer:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread KVStore.cpp -o kvstore-tsan && ./kvstore-tsan

template<typename K, typename V>
class KVStore {
    std::unordered_map<K, V> data_;

    mutable std::shared_mutex mutex_;

public:
    KVStore() = default;
    KVStore(const KVStore&) = delete;
    KVStore& operator=(const KVStore&) = delete;

    std::optional<V> get(const K& key) const {
        std::shared_lock lock(mutex_);

        auto found = data_.find(key);
        if(found == data_.end()) {
            return std::nullopt;
        }

        return found->second;
    }

    void put(K key, V value) {
        std::lock_guard lock(mutex_);

        data_.insert_or_assign(std::move(key), std::move(value));
    }

    bool erase(const K& key) {
        std::lock_guard lock(mutex_);

        return data_.erase(key) > 0;
    }

    std::size_t size() const {
        std::shared_lock lock(mutex_);
        return data_.size();
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
