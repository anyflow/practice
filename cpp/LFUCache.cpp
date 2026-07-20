/*
INTERVIEW QUESTION — Thread-safe LFU Cache (50분)

고정된 수의 항목을 저장하는 in-memory LFU Cache를 구현하라.
같은 frequency의 항목끼리는 LRU 정책으로 퇴출 대상을 결정한다.

제공할 API

    template <typename K, typename V>
    class LFUCache {
    public:
        explicit LFUCache(std::size_t capacity);
        void put(K key, V value);
        std::optional<V> get(const K& key);
        std::size_t size() const;
    };

동작 계약

- 새 항목의 frequency는 1이다.
- 성공한 get()은 value를 반환하고 frequency를 1 증가시킨다.
- 기존 key의 put()은 value를 교체하고 frequency를 1 증가시킨다.
- 새 key로 capacity를 초과하면 frequency가 가장 낮은 항목을 제거한다.
- 같은 frequency에서는 가장 오래전에 사용된 항목을 제거한다.
- capacity가 0이면 아무 항목도 저장하지 않는다.
- get(), put(), size()는 평균 O(1), 저장 공간은 O(capacity)를 목표로 한다.

동시성 계약

- 여러 thread가 하나의 인스턴스에 동시에 get()과 put()을 호출할 수 있다.
- 각 호출은 어떤 유효한 직렬 실행 순서 하나로 설명돼야 한다.
- key, frequency별 최근성 목록, minimum frequency가 서로 어긋나면 안 된다.
- 모든 public 함수가 끝난 시점의 항목 수는 capacity를 넘으면 안 된다.

제출 전 검증

- hit과 miss, frequency 증가, LFU 퇴출
- 같은 frequency에서 LRU tie-break
- 기존 value 갱신과 capacity 0
- sleep 없는 다중 thread 검증과 ThreadSanitizer 실행

Follow-up

1. 평균 O(1)을 위해 어떤 상태를 각각 저장해야 하는가?
   답: key별 value·frequency·list iterator, frequency별 LRU list, 현재 minimum frequency를 저장한다.
2. minimum frequency는 언제 증가하고 언제 1로 초기화되는가?
   답: minimum bucket의 마지막 항목이 승격되면 1 증가하고, 새 key를 삽입하면 1로 초기화한다.
3. frequency별 목록에서 항목을 이동한 뒤 어떤 iterator를 갱신해야 하는가?
   답: Entry의 위치 iterator가 새 frequency list의 노드를 가리켜야 하며 splice를 쓰면 기존 iterator가 그대로 유효하다.
4. get()에도 배타 락이 필요한 이유는 무엇인가?
   답: hit 시 frequency, LRU 순서와 minimum frequency를 변경하므로 배타 락이 필요하다.
5. 하나의 mutex가 병목일 때 sharding하면 어떤 전역 보장을 잃는가?
   답: 별도 전역 동기화 없이는 전체 cache의 정확한 capacity와 전역 LFU·LRU 퇴출 순서를 보장하기 어렵다.
6. 오래된 고빈도 항목이 영구히 남는 cache pollution을 어떻게 완화할 것인가?
   답: 주기적으로 frequency를 감소시키거나 시간 가중치를 적용해 오래된 접근의 영향력을 줄인다.
*/


// ASan/UBSan:
// g++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     LFUCache.cpp -o lfu && ./lfu
//
// ThreadSanitizer:
// g++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread LFUCache.cpp -o lfu-tsan && ./lfu-tsan

#include <optional>
#include <cstddef>
#include <string>
#include <cassert>
#include <vector>
#include <barrier>
#include <chrono>
#include <list>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>

template<typename K, typename V>
class LFUCache {
    using Frequency = std::size_t;
    struct Entry {
        V value;
        Frequency frequency = 1;
        typename std::list<K>::iterator position;
    };

    std::size_t min_frequency_ = 0;
    std::unordered_map<Frequency, std::list<K>> frequencies_;
    std::unordered_map<K, Entry> entries_;

    const std::size_t capacity_;
    mutable std::mutex mutex_;

    void update_frequency(Entry& entry) {
        const auto& [_, old_freq, old_pos] = entry;
        auto new_freq = old_freq + 1;

        auto& old_list = frequencies_.at(old_freq);
        auto& new_list = frequencies_[new_freq];
        new_list.splice(new_list.begin(), old_list, old_pos);

        if(old_list.empty()) {
            if(old_freq == min_frequency_) {
                ++min_frequency_;
            }

            frequencies_.erase(old_freq);
        }

        entry.frequency = new_freq;
    }

public:
    explicit LFUCache(std::size_t capacity)
        : capacity_(capacity) {}

    void put(K key, V value) {
        if(capacity_ == 0) {
            return;
        }

        std::lock_guard lock(mutex_);

        if(auto found = entries_.find(key); found != entries_.end()) {
            found->second.value = std::move(value);
            update_frequency(found->second);
            return;
        }

        if(entries_.size() >= capacity_) {
            auto remove_freq = frequencies_.find(min_frequency_);
            auto remove_key = remove_freq->second.back();
            remove_freq->second.pop_back();
            entries_.erase(remove_key);

            if(remove_freq->second.empty()) {
                frequencies_.erase(remove_freq);
            }
        }

        auto& new_freq = frequencies_[1];
        min_frequency_ = 1;

        new_freq.push_front(std::move(key));
        entries_.try_emplace(new_freq.front(), Entry{std::move(value), 1, new_freq.begin()});
    }

    std::optional<V> get(const K& key) {
        std::lock_guard lock(mutex_);

        auto found = entries_.find(key);
        if(found == entries_.end()) {
            return std::nullopt;
        }

        auto result = found->second.value;
        update_frequency(found->second);
        return result;
    }

    std::size_t size() const {
        std::lock_guard lock(mutex_);

        return entries_.size();
    }
};

void testBasicPutGetAndMiss() {
    LFUCache<int, std::string> cache(2);

    assert(!cache.get(1));
    assert(cache.size() == 0);

    cache.put(1, "A");
    cache.put(2, "B");

    assert(cache.get(1) == "A");
    assert(cache.get(2) == "B");
    assert(cache.size() == 2);
    assert(!cache.get(3));
    assert(cache.size() == 2);
}

void testLeastFrequentlyUsedEviction() {
    LFUCache<int, std::string> cache(2);

    cache.put(1, "A");
    cache.put(2, "B");
    assert(cache.get(1) == "A"); // frequency: 1 -> 2

    cache.put(3, "C"); // frequency 1인 key 2를 제거한다.

    assert(!cache.get(2));
    assert(cache.get(1) == "A");
    assert(cache.get(3) == "C");
    assert(cache.size() == 2);
}

void testLruTieBreak() {
    LFUCache<int, int> cache(2);

    cache.put(1, 10);
    cache.put(2, 20);
    assert(cache.get(1) == 10); // frequency 2, 이 시점에는 key 1이 먼저 사용됨
    assert(cache.get(2) == 20); // frequency 2에서 key 2가 MRU, key 1이 LRU

    cache.put(3, 30);

    assert(!cache.get(1));
    assert(cache.get(2) == 20);
    assert(cache.get(3) == 30);
}

void testUpdateAndZeroCapacity() {
    LFUCache<int, std::string> cache(2);

    cache.put(1, "A");
    cache.put(2, "B");
    cache.put(1, "A2"); // 기존 put도 frequency를 증가시킨다.
    cache.put(3, "C");

    assert(cache.get(1) == "A2");
    assert(!cache.get(2));
    assert(cache.size() == 2);

    LFUCache<int, int> zero(0);
    zero.put(1, 10);
    assert(!zero.get(1));
    assert(zero.size() == 0);
}

void testNewInsertionResetsMinimumFrequency() {
    LFUCache<int, int> cache(2);

    cache.put(1, 10);
    assert(cache.get(1) == 10);
    assert(cache.get(1) == 10); // key 1의 frequency만 3까지 올린다.

    cache.put(2, 20);           // 새 key로 minimum frequency가 1이 된다.
    cache.put(3, 30);           // frequency 1인 key 2를 제거한다.

    assert(cache.get(1) == 10);
    assert(!cache.get(2));
    assert(cache.get(3) == 30);
}

void testRepeatedFrequencyMovementAndMinTracking() {
    LFUCache<int, int> cache(3);

    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);
    assert(cache.get(1) == 10); // key 1: frequency 1 -> 2
    assert(cache.get(1) == 10); // key 1: frequency 2 -> 3
    assert(cache.get(2) == 20); // key 2: frequency 1 -> 2

    cache.put(4, 40);           // 유일한 frequency 1인 key 3을 제거한다.
    assert(!cache.get(3));
    assert(cache.get(4) == 40); // frequency 1이 비어 minimum이 2가 된다.

    cache.put(5, 50);           // frequency 2에서 LRU인 key 2를 제거한다.
    assert(!cache.get(2));
    assert(cache.get(1) == 10);
    assert(cache.get(4) == 40);
    assert(cache.get(5) == 50);
}

void testCapacityOneReplacement() {
    LFUCache<int, int> cache(1);

    cache.put(1, 10);
    assert(cache.get(1) == 10);
    cache.put(2, 20);

    assert(!cache.get(1));
    assert(cache.get(2) == 20);
    assert(cache.size() == 1);
}

void testConcurrentFrequencyUpdates() {
    constexpr int threadCount = 8;
    constexpr int hitsPerThread = 200;

    LFUCache<int, int> cache(2);
    cache.put(1, 10);
    cache.put(2, 20);

    std::barrier startLine(threadCount);
    std::vector<std::thread> threads;
    for (int worker = 0; worker < threadCount; ++worker) {
        threads.emplace_back([&] {
            startLine.arrive_and_wait();
            for (int i = 0; i < hitsPerThread; ++i) {
                assert(cache.get(1) == 10);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    cache.put(3, 30);
    assert(cache.get(1) == 10);
    assert(!cache.get(2));
    assert(cache.get(3) == 30);
}

void testConcurrentAccess() {
    constexpr std::size_t capacity = 32;
    constexpr int threadCount = 8;
    constexpr int operations = 1'000;

    LFUCache<int, int> cache(capacity);
    std::barrier startLine(threadCount);
    std::vector<std::thread> threads;

    for (int worker = 0; worker < threadCount; ++worker) {
        threads.emplace_back([&, worker] {
            startLine.arrive_and_wait();

            for (int i = 0; i < operations; ++i) {
                const int key = (worker * 17 + i) % 64;
                const int expected = key * 10;

                cache.put(key, expected);
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
    for (int key = 0; key < 64; ++key) {
        if (const auto value = cache.get(key)) {
            assert(*value == key * 10);
        }
    }
}

int main() {
    testBasicPutGetAndMiss();
    testLeastFrequentlyUsedEviction();
    testLruTieBreak();
    testUpdateAndZeroCapacity();
    testNewInsertionResetsMinimumFrequency();
    testRepeatedFrequencyMovementAndMinTracking();
    testCapacityOneReplacement();
    testConcurrentFrequencyUpdates();
    testConcurrentAccess();
}
