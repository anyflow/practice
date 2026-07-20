// g++ -std=c++23 -pthread -Wall -fsanitize=address,undefined -fno-omit-frame-pointer -Wextra -g -O0 day9.cpp -o a && ./a
// g++ -std=c++23 -pthread -Wall -fsanitize=thread -fno-omit-frame-pointer -Wextra -g -O0 day9.cpp -o a && ./a

#include <iostream>
#include <cassert>

#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <optional>
#include <vector>
#include <thread>
#include <string>
#include <algorithm>
#include <utility>

template<typename K, typename V>
class KVStore {
    std::unordered_map<K, V> data;
    mutable std::shared_mutex m;

public:
    std::optional<V> get(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(m);
        if(auto it = data.find(key); it != data.end()) { return it->second; }

        return std::nullopt;
    }

    void put(const K& key, const V& value) {
        std::lock_guard<std::shared_mutex> lock(m);
        data.insert_or_assign(key, value);
    }

    bool erase(const K& key) {
        std::lock_guard<std::shared_mutex> lock(m);

        if(auto it = data.find(key); it != data.end()) {
            data.erase(it);
            return true;
        }

        return false;
    }

    std::size_t size() const {
        std::shared_lock<std::shared_mutex> lock(m);

        return data.size();
    }
};

void p2() {
    KVStore<int, int> store;

    std::vector<std::thread> writers;
    for(int i=0; i<2; ++i) {
        writers.emplace_back([&store, i] {
            int start = i * 1000;
            for(int j=start; j<start + 1000; ++j) {
                store.put(j, j * 10);
            }
        });
    }

    std::vector<std::thread> readers;
    for(int i=0; i<8; ++i) {
        readers.emplace_back([&store] {
            while(store.size() == 0) {  // interview-only: reader 검증이 실제로 실행되도록 첫 삽입까지 대기
                std::this_thread::yield();
            }
            for(int j=0; j<2000; ++j) {
                auto val = store.get(j);
                if(val) { assert(*val == j * 10); }
            }
        });
    }

    for(auto& t: writers) { t.join(); }
    for(auto& t: readers) { t.join(); }

    auto size = store.size();
    assert(size == 2000);
    for(int i=0; i<(int)size; ++i) {
        assert(*store.get(i) == i * 10);
    }

    assert(!store.erase(2000));
    assert(store.erase(1999) && store.get(1999) == std::nullopt);
}

std::vector<std::vector<std::string>> group_anagrams(const std::vector<std::string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> items;

    for(const auto& str: strs) {
        auto sorted = str;
        std::sort(sorted.begin(), sorted.end());

        items[sorted].push_back(str);
    }

    std::vector<std::vector<std::string>> ret;
    for(auto& item: items) { ret.push_back(std::move(item.second)); }

    return ret;
}

int main() {
    p2();

    std::vector<std::string> strs{"eat", "tea", "tan", "ate", "nat", "bat"};
    auto ret = group_anagrams(strs);

    auto normalize = [](auto groups) {
        for (auto& group : groups)
            std::sort(group.begin(), group.end());

        std::sort(groups.begin(), groups.end());
        return groups;
    };

    std::vector<std::vector<std::string>> expected{
        {"eat", "tea", "ate"},
        {"tan", "nat"},
        {"bat"}
    };

    assert(normalize(ret) == normalize(expected));

    return 0;
}