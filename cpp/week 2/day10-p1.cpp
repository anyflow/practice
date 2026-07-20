#include <iostream>
#include <cassert>
#include <string>
#include <optional>

#include <list>
#include <unordered_map>
#include <mutex>
#include <utility> // 수정: std::move를 선언한 헤더를 직접 포함



// template<typename K, typename V>
// class LRUCache {
//     using Entry = std::pair<K, V>;
//     using List = std::list<Entry>;
//     using Map = std::unordered_map<K, typename List::iterator>;

//     std::size_t capacity;
//     List items;
//     Map index;

// public:
//     explicit LRUCache(std::size_t capacity): capacity(capacity) {}

//     std::optional<V> get(const K& key) {
//         if(capacity == 0) { return std::nullopt; }

//         auto it = index.find(key);
//         if(it == index.end()) { return std::nullopt; }

//         items.splice(items.begin(), items, it->second);
//         return it->second->second;
//     }

//     void put(K key, V value) {
//         if(capacity == 0) { return; }

//         if(auto it = index.find(key); it != index.end()) {
//             items.erase(it->second);
//             index.erase(it);
//         }
//         else if(index.size() == capacity) {
//             index.erase(items.back().first);
//             items.pop_back();
//         }

//         items.push_front(Entry{std::move(key), std::move(value)});
//         index.insert_or_assign(items.front().first, items.begin());
//         assert(items.size() == index.size() && items.size() <= capacity); // 수정: 핵심 불변식 검사
//     }
// };

// Best Practice: list가 항목과 최근성 순서를 소유하고 map은 list iterator만 저장한다.
template<typename K, typename V>
class LRUCache {
    using Entry = std::pair<K, V>;
    using List = std::list<Entry>;
    using Map = std::unordered_map<K, typename List::iterator>;

    std::size_t capacity_;
    List items_;
    Map index_;

public:
    explicit LRUCache(std::size_t capacity): capacity_(capacity) {}

    // index_의 iterator는 items_를 가리키므로 단순 복사를 금지한다.
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    std::optional<V> get(const K& key) {
        auto found = index_.find(key);
        if(found == index_.end()) { return std::nullopt; }

        items_.splice(items_.begin(), items_, found->second);
        return found->second->second;
    }

    void put(K key, V value) {
        if(capacity_ == 0) { return; }

        if(auto found = index_.find(key); found != index_.end()) {
            found->second->second = std::move(value);
            items_.splice(items_.begin(), items_, found->second);
            return;
        }

        if(index_.size() == capacity_) {
            index_.erase(items_.back().first);
            items_.pop_back();
        }

        items_.emplace_front(std::move(key), std::move(value));
        index_.emplace(items_.front().first, items_.begin());

        assert(items_.size() == index_.size());
        assert(items_.size() <= capacity_);
    }
};

int main() {
    LRUCache<int, int> cache(100'000);

    for(int i=0; i<100'000; ++i) {
        cache.put(i, i * 10);
    }

    for(int i=0; i<100'000; ++i) {
        auto val = cache.get(i);
        assert(val);
        assert(*val == i * 10);
    }

    assert(cache.get(100'000) == std::nullopt);

    cache.put(100'000, 100'000 * 10);
    assert(cache.get(0) == std::nullopt);

    /* 기존 테스트: key 0이 이미 퇴출되어 기존 key 갱신을 검증하지 못함
    cache.put(0, 1);
    assert(cache.get(0) == 1 && cache.get(1) == std::nullopt);
    */

    // 수정: 여유 공간이 있을 때 기존 key의 value와 최근성이 함께 갱신되는지 검증
    LRUCache<int, int> update(3);
    update.put(1, 10);
    update.put(2, 20);
    update.put(1, 11);
    update.put(3, 30);
    update.put(4, 40);
    assert(update.get(2) == std::nullopt);
    assert(update.get(1) == 11);

    // 수정: 성공한 get 이후 최근성이 바뀌어 올바른 key가 퇴출되는지 검증
    LRUCache<int, int> recency(2);
    recency.put(1, 10);
    recency.put(2, 20);
    assert(recency.get(1) == 10);
    recency.put(3, 30);
    assert(recency.get(2) == std::nullopt);

    // 수정: 이동한 std::string key를 map에 잘못 저장하는 회귀를 검증
    LRUCache<std::string, int> string_key(1);
    string_key.put("key", 1);
    assert(string_key.get("key") == 1);

    LRUCache<int, int> cache2(0);
    cache2.put(0, 1);
    assert(cache2.get(0) == std::nullopt);

    return 0;
}
