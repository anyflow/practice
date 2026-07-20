#include <cassert>
#include <chrono>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <tuple>
#include <mutex>
#include <vector>
#include <thread>
#include <numeric>
#include <algorithm>
#include <iostream>

using TimePoint = std::chrono::steady_clock::time_point;
// using Entry = std::tuple<double, TimePoint, std::mutex>; // tokens_, last_refill_;

struct Entry {
    double tokens_;
    TimePoint last_refill_;
    std::mutex mutex_;

    Entry(double tokens, TimePoint last_refill):
        tokens_(tokens),
        last_refill_(last_refill)
        {}
};

using Buckets = std::unordered_map<std::string, Entry>;


class TokenBucket {
    double refill_per_sec_;
    double capacity_;
    Buckets buckets_;
    mutable std::mutex mutex_;

public:
    TokenBucket(double refill_per_sec, double capacity)
        : refill_per_sec_(refill_per_sec),
          capacity_(capacity)
        {}

    bool allow(const std::string& key, TimePoint now) {
        std::unique_lock<std::mutex> global_lock(mutex_);
        auto it = buckets_.find(key);
        if(it == buckets_.end()) {
            it = buckets_.try_emplace(key, capacity_, TimePoint{}).first;
        }
        Entry& entry = it->second;
        global_lock.unlock();

        std::unique_lock<std::mutex> lock(entry.mutex_);

        auto elapsed = std::chrono::duration<double>(now - entry.last_refill_).count();
        entry.tokens_ = std::min(capacity_, entry.tokens_ + elapsed * refill_per_sec_);
        entry.last_refill_ = now;

        if(entry.tokens_ < 1.0) {
            return false;
        }

        entry.tokens_ -= 1.0;
        return true;
    }

    std::size_t size() const {
        std::unique_lock<std::mutex> global_lock(mutex_);
        return buckets_.size();
    }
};

void p1() {
    using namespace std::chrono_literals;

    const TimePoint t0{};
    TokenBucket bucket(2.0, 5.0);

    const std::string user_1 = "A";

    assert(bucket.allow(user_1, t0));                    // 4.0 tokens
    assert(bucket.allow(user_1, t0));                    // 3.0 tokens
    assert(bucket.allow(user_1, t0));                    // 2.0 tokens
    assert(bucket.allow(user_1, t0));                    // 1.0 tokens
    assert(bucket.allow(user_1, t0));                    // 0.0 tokens
    assert(!bucket.allow(user_1, t0));                   // insufficient tokens
    assert(!bucket.allow(user_1, t0 + 250ms));           // 0.5 tokens
    assert(bucket.allow(user_1, t0 + 500ms));            // refill to 1.0, then consume

    const auto later = t0 + 10s + 500ms;
    assert(bucket.allow(user_1, later));                 // capacity까지 refill
    assert(bucket.allow(user_1, later));
    assert(bucket.allow(user_1, later));
    assert(bucket.allow(user_1, later));
    assert(bucket.allow(user_1, later));
    assert(!bucket.allow(user_1, later));                // capacity 초과 금지
}

void p2() {
    using namespace std::chrono_literals;

    const TimePoint t0{};
    TokenBucket bucket(2.0, 2.0);

    const std::string alice = "alice";
    const std::string bob = "bob";

    assert(bucket.allow(alice, t0));
    assert(bucket.allow(alice, t0));
    assert(!bucket.allow(alice, t0));
    assert(bucket.allow(bob, t0));
    assert(bucket.allow(alice, t0 + 500ms));


    std::vector<int> counts2(8, 0);
    TokenBucket bucket2(2.0, 5.0);
    std::vector<std::thread> threads2;
    for(std::size_t i=0; i<8; ++i) {
        threads2.emplace_back([&bucket2, &t0, &counts2, i]() {
            for(std::size_t j=0; j<1000; ++j) {
                if(bucket2.allow("shared", t0)) {
                    ++counts2[i];
                }
            }
        });
    }

    for(auto& t: threads2) {
        t.join();
    }

    std::size_t sum2 = 0;
    for(auto i: counts2) { sum2 += i; }
    assert(sum2 == 5);


    std::vector<int> counts3A(8, 0);
    std::vector<int> counts3B(8, 0);
    TokenBucket bucket3(2.0, 5.0);
    std::vector<std::thread> threads3;
    for(std::size_t i=0; i<8; ++i) {
        threads3.emplace_back([&bucket3, &t0, &counts3A, &counts3B, i]() {
            for(std::size_t j=0; j<1000; ++j) {
                if(bucket3.allow("A", t0)) {
                    ++counts3A[i];
                }
                if(bucket3.allow("B", t0)) {
                    ++counts3B[i];
                }
            }
        });
    }

    for(auto& t: threads3) {
        t.join();
    }

    assert(bucket3.size() == 2);

    std::size_t sum3A = 0;
    for(auto i: counts3A) { sum3A += i; }
    assert(sum3A == 5);

    std::size_t sum3B = 0;
    for(auto i: counts3B) { sum3B += i; }
    assert(sum3B == 5);
}

/* 기존 구현 보존
std::size_t minSubarrayLen(std::int64_t target, const std::vector<int>& nums) {
    if(nums.size() == 0 && target != 0) {
        return 0;
    }

    auto left = nums.begin(), right = nums.begin();
    std::size_t count = nums.size() + 1;
    std::int64_t sum = nums[0];

    while(right != nums.end() && left <= right) {
        if(sum >= target) {
            count = std::min(count, static_cast<std::size_t>(right + 1 - left));
            sum -= *left;
            ++left;
        }
        else {
            ++right;
            if(right != nums.end()) {
                sum += *right;
            }
        }
    }

    return count == nums.size() + 1 ? 0: count;
}
*/

std::size_t minSubarrayLen(std::int64_t target, const std::vector<int>& nums) {
    std::size_t left = 0;
    std::size_t right = 0; // 현재 구간은 [left, right). 현재 구간의 끝 다음 위치
    std::size_t best = nums.size() + 1;
    std::int64_t sum = 0;

    while(right < nums.size()) {
        sum += nums[right++];

        while(sum >= target) {
            best = std::min(best, right - left);
            sum -= nums[left++];
        }
    }

    return best == nums.size() + 1 ? 0 : best;
}

void warmingup() {
    assert(minSubarrayLen(7, {2, 3, 1, 2, 4, 3}) == 2);
    assert(minSubarrayLen(4, {1, 4, 4}) == 1);
    assert(minSubarrayLen(11, {1, 1, 1, 1, 1, 1, 1, 1}) == 0);
    assert(minSubarrayLen(3, {}) == 0);
    assert(minSubarrayLen(6, {1, 2, 3}) == 3);
}

int main() {

    p1();
    p2();
    warmingup();

    return 0;
}
