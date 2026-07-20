#include <unordered_map>
#include <list>
#include <utility>
#include <optional>
#include <cassert>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <stack>
#include <string>
#include <array>

template<typename K, typename V>
class LRUCache {
    using Entry = std::pair<K, V>;
    using List = std::list<Entry>;
    using Map = std::unordered_map<K, typename List::iterator>;

    std::size_t capacity_;
    List items_;
    Map index_;
    mutable std::mutex mutex_;

public:
    explicit LRUCache(std::size_t capacity): capacity_(capacity) {}

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    void put(K key, V value) {
        std::lock_guard<std::mutex> lock(mutex_);

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

        items_.push_front(Entry{std::move(key), std::move(value)});
        index_.emplace(items_.begin()->first, items_.begin());

        assert(index_.size() <= capacity_);
        assert(index_.size() == items_.size());
    }

    std::optional<V> get(const K& key) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto found = index_.find(key);
        if(found == index_.end()) { return std::nullopt; }

        items_.splice(items_.begin(), items_, found->second);
        return found->second->second;
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return index_.size();
    }
};

bool isValid(const std::string& s) {
    // using char_arr3 = const char[3];

    // char_arr3 opens = {'[', '{', '('};
    // char_arr3 closes = {']', '}', ')'};

    // auto index = [](const char& ch, char_arr3& arr) {
    //     for(std::size_t i=0; i<std::size(arr); ++i) {
    //         if(ch == arr[i]) { return (int)i; }
    //     }

    //     return -1;
    // };

    // std::stack<char> checker;

    // for(const char& ch: s) {
    //     if(auto open_index = index(ch, opens); open_index != -1) {
    //         checker.push(ch);
    //     }
    //     else if(auto close_index = index(ch, closes); close_index != -1) {
    //         if(checker.empty()) { return false; }
    //         if(index(checker.top(), opens) != close_index) { return false; }

    //         checker.pop();
    //     }
    // }

    // return checker.empty();

    std::stack<char> opened;

    for(char ch : s) {
        char expected;

        switch(ch) {
            case '(':
            case '[':
            case '{':
                opened.push(ch);
                continue;
            case ')': expected = '('; break;
            case ']': expected = '['; break;
            case '}': expected = '{'; break;
            default: return false;
        }

        if(opened.empty() || opened.top() != expected) {
            return false;
        }
        opened.pop();
    }

    return opened.empty();
}

int main() {
    const std::size_t size = 32;
    LRUCache<int, int> cache(size);

    std::vector<std::thread> threads;

    for(int i=0; i<4; ++i) {
        threads.emplace_back([&cache] {
            for(int j=0; j<1000; ++j) {
                const int key = j % 64;
                const int value = key * 10;
                cache.put(key, value);
                auto retrieved = cache.get(key);
                assert(retrieved == value || retrieved == std::nullopt);
            }
        });
    }

    for(auto& t: threads) { t.join(); }

    assert(cache.size() <= size);

// warming-up drill

    assert(isValid(""));
    assert(isValid("()[]{}"));
    assert(isValid("([{}])"));
    assert(!isValid("(]"));
    assert(!isValid("([)]"));
    assert(!isValid("]"));
    assert(!isValid("(("));

    return 0;
}