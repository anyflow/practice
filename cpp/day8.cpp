// g++ -std=c++23 -pthread -Wall -fsanitize=address,undefined -fno-omit-frame-pointer -Wextra day8.cpp -o a && ./a

#include <thread>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <queue>
#include <utility>
#include <vector>
#include <iostream>
#include <cassert>


template<typename T>
class TSQueue {
    std::queue<T> q;
    std::mutex m;
    std::condition_variable cv;
    bool closed{false};

public:
    explicit TSQueue() {}

    bool push(T value) {
        {
            std::lock_guard<std::mutex> l(m);
            if(closed) { return false; }

            q.push(std::move(value));
        }
        cv.notify_one();
        return true;
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> ul(m);
        cv.wait(ul, [&]{ return !q.empty() || closed; });

        if(q.empty()) { return std::nullopt; }

        T ret = std::move(q.front());
        q.pop();
        return ret;
    }

    bool try_pop(T& out) {
        std::lock_guard<std::mutex> l(m);

        if(q.empty()) { return false; }
        else {
            out = std::move(q.front());
            q.pop();

            return true;
        }
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> l(m);
            closed = true;
        }

        cv.notify_all();
    }
};

void p1() {
// P1. TSQueue 백지 구현**

// - 위 API를 참고 없이 20분 안에 구현한다.
// - `std::queue`, `std::mutex`, `std::condition_variable`만 사용한다.
// - 생산자 3개가 서로 다른 범위의 정수 100개씩 넣는다.
// - 소비자 2개가 shutdown까지 값을 꺼낸다.
// - 총 300개가 중복·누락 없이 처리됐는지 검증한다.

    TSQueue<int> queue;
    std::vector<int> seen(300, 0);
    std::mutex m;


    std::vector<std::thread> producers;
    for(int i=0; i<3; ++i) {
        producers.emplace_back([&queue, i] {
            int start = i * 100;
            for(int j=start; j<start + 100; ++j) { queue.push(j); }
        });
    }

    std::vector<std::thread> consumers;
    for(int i=0; i<2; ++i) {
        consumers.emplace_back([&] {
            while(auto item = queue.pop()) {
                std::lock_guard l(m);
                seen[*item] += 1;
            }
        });
    }

    for(auto& t: producers) { t.join(); }
    queue.shutdown();
    assert(queue.push(1) == false);

    for(auto& t: consumers) { t.join(); }

    for(size_t i=0; i<300; ++i) {
        assert(seen[i] == 1);
    }

    TSQueue<int> queue2;
    queue2.push(1);

    int value = 0;
    assert(queue2.try_pop(value));
    assert(value == 1);
    assert(queue2.try_pop(value) == false);
}

std::optional<std::pair<int, int>>
twosum(const std::vector<int>& nums, int target) {
// Two Sum**을 `unordered_map`으로 풀고, 시간 `O(n)`·공간 `O(n)`인 이유를 말한다.
// 정수 배열과 `target`이 주어지면 합이 `target`인 서로 다른 두 원소의 index를 반환한다. 답이 없으면 `std::nullopt`를 반환한다.

// ```text
// nums = [2, 7, 11, 15], target = 9
// 결과 = (0, 1)
// ```

    std::unordered_map<int, int> val_index;
    for(size_t i=0; i<nums.size(); ++i) { val_index[nums[i]] = i; }

    int other_val = 0;
    for(size_t i=0; i<nums.size(); ++i) {
        other_val = target - nums[i];

        if(auto it = val_index.find(other_val); it != val_index.end() && it->second != (int)i) {
            // return std::make_pair(i, it->second);
            return {{i, it->second}};
        }
    }

    return std::nullopt;
}

int main() {
    p1();

    std::vector<int> nums{2, 7, 11, 15};
    auto twosum_ret = twosum(nums, 9);

    assert(twosum_ret->first == 0 || twosum_ret->first == 1);
    assert(twosum_ret->second == 1 || twosum_ret->second == 0);
    assert(nums[twosum_ret->first] + nums[twosum_ret->second] == 9);

    return 0;
}