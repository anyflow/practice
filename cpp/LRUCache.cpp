/*
INTERVIEW QUESTION — Thread-safe LRU Cache (45분)

고정된 수의 항목을 저장하는 in-memory LRU Cache를 구현하라.
사용할 자료구조와 내부 동기화 방식은 지원자가 결정한다.

제공할 API

    template <typename K, typename V>
    class LRUCache {
    public:
        explicit LRUCache(std::size_t capacity);
        void put(K key, V value);
        std::optional<V> get(const K& key);
        std::size_t size() const;
    };

동작 계약

- get(key)는 key가 있으면 value를 반환하고 그 항목을 MRU로 만든다.
- get(key)가 실패하면 nullopt를 반환하며 최근 사용 순서는 바뀌지 않는다.
- put(key, value)는 기존 key의 값과 최근성을 갱신한다.
- 새 key로 capacity를 초과하면 LRU 항목 하나를 제거한다.
- capacity가 0이면 아무 항목도 저장하지 않는다.
- 같은 key는 최대 하나만 존재해야 한다.
- get(), put(), size()는 평균 O(1), 저장 공간은 O(capacity)를 목표로 한다.

동시성 계약

- 여러 thread가 하나의 인스턴스에 동시에 get()과 put()을 호출할 수 있다.
- 각 호출은 어떤 유효한 직렬 실행 순서의 한 지점에서 완료된 것으로 설명돼야 한다.
- 호출 도중의 부분 갱신 상태가 다른 thread에 보여서는 안 된다.
- 모든 연산이 끝났을 때 항목 수는 capacity를 초과하면 안 된다.

제출 전 검증

- cache hit과 miss
- 성공한 get() 이후 최근성 변경
- capacity 초과 시 LRU 퇴출
- 기존 key의 값과 최근성 갱신
- capacity 0
- sleep 없는 다중 thread 검증과 ThreadSanitizer 실행

Follow-up

1. get()에도 배타 락이 필요한 이유는 무엇인가?
2. 최근성 순서와 key 조회 자료구조 사이의 핵심 불변식은 무엇인가?
3. 항목 이동 후 iterator는 계속 유효한가?
4. 하나의 락이 병목일 때 sharding하면 어떤 보장을 잃을 수 있는가?
5. 삽입 중 예외가 발생해도 내부 상태를 일관되게 유지하려면 어떻게 하는가?
6. TTL을 추가하면서 만료 항목을 효율적으로 찾으려면 무엇이 더 필요한가?
*/


// ASan/UBSan:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     LRUCache.cpp -o lru && ./lru
//
// ThreadSanitizer:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread LRUCache.cpp -o lru-tsan && ./lru-tsan

#include <optional>
#include <barrier>
#include <mutex>
#include <list>
#include <unordered_map>
#include <cassert>
#include <vector>
#include <thread>
#include <utility>
#include <string>

template<typename K, typename V>
class LRUCache {
    using Entry = std::pair<K, V>;
    using Entries = std::list<Entry>;

    const std::size_t capacity_;
    Entries entries_;
    std::unordered_map<K, typename Entries::iterator> index_;

    mutable std::mutex mutex_;

  public:
    explicit LRUCache(std::size_t capacity): capacity_(capacity) {}

    std::optional<V> get(const K& key) {
        /*
        1. if not exist in index, return null
        3. move the entry into the front in entries_
        4. return entries_.front()
        */

        std::lock_guard lock(mutex_);

        auto found = index_.find(key);
        if(found == index_.end()) {
            return std::nullopt;
        }

        entries_.splice(entries_.begin(), entries_, found->second);
        return entries_.front().second;
    }

    void put(K key, V value) {
        /*
        1. upsert
        2. move entry into the front in entries_
        3. if index size > capacity; delete entries_.back(), index
        */

        if(capacity_ == 0) {
            return;
        }

        std::lock_guard lock(mutex_);

        if(auto found = index_.find(key); found != index_.end()) {
            found->second->second = std::move(value);
            entries_.splice(entries_.begin(), entries_, found->second);
            return;
        }

        entries_.push_front(std::make_pair(std::move(key), std::move(value)));
        index_.try_emplace(entries_.begin()->first, entries_.begin());

        if(index_.size() > capacity_) {
            index_.erase(entries_.back().first);
            entries_.pop_back();
        }
    }

    std::size_t size() const {
        std::lock_guard lock(mutex_);

        assert(entries_.size() == index_.size());
        return index_.size();
    }
};

void testBasicBehavior() {
    LRUCache<int, std::string> cache(2);

    assert(!cache.get(1));

    cache.put(1, "A");
    cache.put(2, "B");

    auto value = cache.get(1); // 1을 MRU로 만든다: [1, 2]
    assert(value && *value == "A");

    cache.put(3, "C"); // LRU였던 2가 제거된다: [3, 1]
    assert(!cache.get(2));

    cache.put(1, "A2"); // 값과 최근성을 함께 갱신한다: [1, 3]
    cache.put(4, "D");  // LRU였던 3이 제거된다: [4, 1]

    assert(!cache.get(3));
    value = cache.get(1);
    assert(value && *value == "A2");
    assert(cache.size() == 2);
}

void testZeroCapacity() {
    LRUCache<int, int> cache(0);

    cache.put(1, 10);

    assert(!cache.get(1));
    assert(cache.size() == 0);
}

void testConcurrentAccess() {
    constexpr std::size_t capacity = 32;
    constexpr int threadCount = 4;
    constexpr int operations = 1'000;

    LRUCache<int, int> cache(capacity);
    std::barrier startLine(threadCount);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (int worker = 0; worker < threadCount; ++worker) {
        threads.emplace_back([&, worker] {
            startLine.arrive_and_wait();

            for (int i = 0; i < operations; ++i) {
                const int key = (worker * 17 + i) % 64;
                const int expected = key * 10;

                cache.put(key, expected);

                // put()과 get() 사이에 다른 thread가 이 key를 퇴출할 수 있다.
                if (const auto value = cache.get(key)) {
                    assert(*value == expected);
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    assert(cache.size() <= capacity);

    // 남아 있는 모든 항목은 완전히 기록된 유효한 값이어야 한다.
    for (int key = 0; key < 64; ++key) {
        if (const auto value = cache.get(key)) {
            assert(*value == key * 10);
        }
    }
}

int main() {
    testBasicBehavior();
    testZeroCapacity();
    testConcurrentAccess();
}
