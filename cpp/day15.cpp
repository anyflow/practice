#include <cassert>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>
#include <functional>
#include <set>
#include <atomic>
#include <thread>
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <queue>

using TwoSumResult = std::optional<std::pair<std::size_t, std::size_t>>;
// using TwoSum = std::function<TwoSumResult(const std::vector<int>& nums, int
// target)>;
using TwoSum = TwoSumResult (*)(const std::vector<int>& nums, int target);

void assertTwoSum(const TwoSum& twoSum, const std::vector<int>& nums, int target) {
    auto result = twoSum(nums, target);
    assert(result.has_value());

    auto [first, second] = *result;
    assert(first < nums.size() && second < nums.size());
    assert(first != second);
    assert(static_cast<long long>(nums[first]) + nums[second] == target);
}

void p1() {
    auto twoSum = [](const std::vector<int>& nums, int target) -> TwoSumResult {
        std::unordered_map<int, std::size_t> valkey;

        for (std::size_t i = 0; i<nums.size(); ++i) {
            int counterpart = target - nums[i];
            if (auto it = valkey.find(counterpart); it != valkey.end()) {
                return std::make_pair(it->second, i);
            }
            valkey.emplace(nums[i], i);
        }

        return std::nullopt;
    };

    assertTwoSum(twoSum, {2, 7, 11, 15}, 9);
    assertTwoSum(twoSum, {3, 3}, 6);
    assert(!twoSum({1, 2, 3}, 10).has_value());
}

void p2() {
    auto twoSumSorted = [](const std::vector<int>& nums, int target) -> TwoSumResult {
        if(nums.size() < 2) {
            return std::nullopt;
        }

        std::size_t left = 0, right = nums.size() - 1;

        while(left < right) {
            auto sum = static_cast<long long>(nums[left]) + nums[right];
            if(sum == target) {
                return std::make_pair(left, right);
            }
            else if (sum < target) {
                ++left;
            }
            else {
                --right;
            }
        }

        return std::nullopt;
    };

    assertTwoSum(twoSumSorted, {2, 7, 11, 15}, 9);
    assertTwoSum(twoSumSorted, {-4, -1, 0, 3, 10}, 6);
    assert(!twoSumSorted({1, 2, 3}, 10).has_value());
}

void p3() {
    auto threeSum = [](std::vector<int>& nums) -> std::vector<std::vector<int>> {
        if(nums.size() < 3) {
            return {};
        }

        std::sort(nums.begin(), nums.end());
        std::vector<std::vector<int>> result;

        for (std::size_t i = 0; i + 2 < nums.size(); ++i) {
            if (i > 0 && nums[i] == nums[i - 1]) {
                continue;
            }

            std::size_t left = i + 1;
            std::size_t right = nums.size() - 1;

            while (left < right) {
                long long sum =
                    static_cast<long long>(nums[i])
                    + nums[left]
                    + nums[right];

                if (sum == 0) {
                    result.push_back({nums[i], nums[left], nums[right]});
                    ++left;
                    --right;

                    while (left < right && nums[left] == nums[left - 1]) {
                        ++left;
                    }
                    while (left < right && nums[right] == nums[right + 1]) {
                        --right;
                    }
                } else if (sum < 0) {
                    ++left;
                } else {
                    --right;
                }
            }
        }

        return result;
    };

    auto normalize = [](const std::vector<std::vector<int>>& result) {
        std::multiset<std::multiset<int>> normalized;
        for (const auto& triplet : result) {
            normalized.emplace(triplet.begin(), triplet.end());
        }
        return normalized;
    };

    std::vector<int> nums1{-1, 0, 1, 2, -1, -4};
    std::multiset<std::multiset<int>> expected1{
        std::multiset<int>{-1, -1, 2},
        std::multiset<int>{-1, 0, 1},
    };
    assert(normalize(threeSum(nums1)) == expected1);

    std::vector<int> nums2{0, 0, 0, 0};
    std::multiset<std::multiset<int>> expected2{
        std::multiset<int>{0, 0, 0},
    };
    assert(normalize(threeSum(nums2)) == expected2);

    std::vector<int> nums3{1, 2, -2, -1};
    assert(threeSum(nums3).empty());

    std::vector<int> nums4{};
    assert(threeSum(nums4).empty());
}

class ThreadSafeQueue {
    std::queue<int> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = false;

public:
    bool push(int value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) {
                return false;
            }
            queue_.push(value);
        }
        cv_.notify_one();
        return true;
    }

    std::optional<int> waitPop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return closed_ || !queue_.empty(); });

        if (queue_.empty()) {
            return std::nullopt;
        }

        int value = queue_.front();
        queue_.pop();
        return value;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        cv_.notify_all();
    }
};

void system_review() {
    // 1. shutdown 전에 들어온 원소를 FIFO로 drain하고, 이후 push를 거부한다.
    {
        ThreadSafeQueue queue;
        assert(queue.push(1));
        assert(queue.push(2));

        queue.shutdown();
        assert(queue.waitPop() == 1);
        assert(queue.waitPop() == 2);
        assert(!queue.waitPop().has_value());
        assert(!queue.push(3));

        queue.shutdown(); // idempotent
    }

    // 2. 빈 queue에서 기다리는 consumer도 shutdown 후 nullopt로 종료한다.
    {
        ThreadSafeQueue queue;
        std::optional<int> result;
        std::thread consumer([&] {
            result = queue.waitPop();
        });

        queue.shutdown();
        consumer.join();
        assert(!result.has_value());
    }

    // 3. 여러 producer와 consumer 사이에서 원소가 중복·누락되지 않는다.
    {
        constexpr int producerCount = 4;
        constexpr int consumerCount = 3;
        constexpr int valuesPerProducer = 250;
        constexpr int totalValues = producerCount * valuesPerProducer;

        ThreadSafeQueue queue;
        std::vector<std::vector<int>> consumed(consumerCount);
        std::vector<std::thread> consumers;
        for (int i = 0; i < consumerCount; ++i) {
            consumers.emplace_back([&, i] {
                while (auto value = queue.waitPop()) {
                    consumed[i].push_back(*value);
                }
            });
        }

        std::atomic<int> accepted = 0;
        std::vector<std::thread> producers;
        for (int producer = 0; producer < producerCount; ++producer) {
            producers.emplace_back([&, producer] {
                const int begin = producer * valuesPerProducer;
                for (int value = begin; value < begin + valuesPerProducer; ++value) {
                    if (queue.push(value)) {
                        ++accepted;
                    }
                }
            });
        }

        for (auto& producer : producers) {
            producer.join();
        }
        queue.shutdown();
        for (auto& consumer : consumers) {
            consumer.join();
        }

        assert(accepted == totalValues);
        std::vector<int> counts(totalValues, 0);
        for (const auto& values : consumed) {
            for (int value : values) {
                assert(0 <= value && value < totalValues);
                ++counts[value];
            }
        }
        for (int count : counts) {
            assert(count == 1);
        }
    }

}

int main() {
    p1();
    p2();
    p3();

    system_review();

    return 0;
}
