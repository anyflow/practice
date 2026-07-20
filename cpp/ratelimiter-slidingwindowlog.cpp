/*
INTERVIEW QUESTIONS — Sliding Window (각 20~30분)

P1. Longest Unique Substring

문자열에서 중복 문자가 없는 가장 긴 연속 substring의 길이를 반환하라.

    std::size_t longestUniqueSubstring(const std::string& text);

- [left, right) window를 유지한다.
- 평균 O(n) 시간과 O(k) 추가 공간을 목표로 한다.
- 빈 문자열과 window 왼쪽에 있는 과거 중복을 처리한다.

P2. Keyed Sliding Window Log Rate Limiter

각 key에 대해 최근 window 안에서 최대 limit개의 요청만 허용하라.

    class SlidingWindowLogRateLimiter {
    public:
        using TimePoint = std::chrono::steady_clock::time_point;

        SlidingWindowLogRateLimiter(std::size_t limit,
                                    std::chrono::milliseconds window);
        bool allow(const std::string& key, TimePoint now);
    };

- 허용된 요청의 timestamp만 저장한다.
- 현재 window는 (now - window, now]이며 경계의 timestamp는 만료된다.
- key별 상태는 독립적이고 모든 호출은 thread-safe하다.
- 같은 key의 시간이 역행하면 마지막으로 처리한 시각으로 평가한다.
- 실제 sleep 없이 초기 제한, 만료, key 독립성, 동시 요청을 검증한다.

Sliding Window 공통 원리

- right에서 새 원소나 요청을 확인한다.
- 조건을 만족할 때까지 left의 원소를 제거한다.
- 유효한 window의 상태나 길이로 결과를 결정한다.
*/

// ASan/UBSan:
// clang++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     SlidingWindow.cpp -o sliding-window && ./sliding-window
//
// ThreadSanitizer:
// clang++ -std=c++20 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread SlidingWindow.cpp -o sliding-window-tsan && ./sliding-window-tsan

#include <algorithm>
#include <atomic>
#include <barrier>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::size_t longestUniqueSubstring(const std::string& text) {
    std::unordered_set<char> window;
    std::size_t left = 0;
    std::size_t right = 0;
    std::size_t longest = 0;

    // window [left, right)는 항상 중복 문자가 없다.
    while (right < text.size()) {
        // 새 문자가 이미 window에 있으면 그 문자가 제거될 때까지 left를 이동한다.
        while(window.find(text[right]) != window.end()) {
            window.erase(text[left]);
            ++left;
        }

        // text[right]를 포함한 뒤 exclusive 경계 right를 한 칸 이동한다.
        window.insert(text[right]);
        ++right;
        longest = std::max(longest, right - left);
    }

    return longest;
}

class RateLimiter {
public:
    using TimePoint = std::chrono::steady_clock::time_point;

private:
    struct Log {
        explicit Log(TimePoint now)
            : last_seen(now) {}

        std::deque<TimePoint> accepted; // 현재 window 안에서 허용된 요청 시각
        TimePoint last_seen;            // 시간 역행을 막기 위한 마지막 처리 시각
    };

    const std::size_t limit_;
    const std::chrono::milliseconds window_;
    std::unordered_map<std::string, Log> logs_;
    std::mutex mutex_;

public:
    RateLimiter(std::size_t limit,
                                std::chrono::milliseconds window)
        : limit_(limit)
        , window_(window) {
        if (window_ <= std::chrono::milliseconds::zero()) {
            throw std::invalid_argument("window must be positive");
        }
    }

    bool allow(const std::string& key, TimePoint now) {
        if (limit_ == 0) {
            return false;
        }

        std::lock_guard lock(mutex_);

        // key마다 독립된 요청 로그를 생성하거나 가져온다.
        auto [found, inserted] = logs_.try_emplace(key, now);
        auto& log = found->second;

        // 역행한 입력 시각은 마지막으로 처리한 시각으로 올려 timestamp 순서를 유지한다.
        if (!inserted) {
            now = std::max(now, log.last_seen);
            log.last_seen = now;
        }

        // 현재 window (now - window_, now] 밖의 오래된 요청을 left에서 제거한다.
        const auto cutoff = now - window_;
        while (!log.accepted.empty() && log.accepted.front() <= cutoff) {
            log.accepted.pop_front();
        }

        // window 안에 이미 limit개의 요청이 있으면 현재 요청은 기록하지 않고 거부한다.
        if (log.accepted.size() >= limit_) {
            return false;
        }

        // 허용된 요청만 right에 추가한다.
        log.accepted.push_back(now);
        return true;
    }
};

void testLongestUniqueSubstring() {
    assert(longestUniqueSubstring("abcabcbb") == 3);
    assert(longestUniqueSubstring("bbbbb") == 1);
    assert(longestUniqueSubstring("pwwkew") == 3);
    assert(longestUniqueSubstring("abba") == 2);
    assert(longestUniqueSubstring("") == 0);
    assert(longestUniqueSubstring("a") == 1);
}

void testSlidingWindowLogBoundaries() {
    using namespace std::chrono_literals;

    const RateLimiter::TimePoint t0{};
    RateLimiter limiter(3, 1s);

    assert(limiter.allow("alice", t0));
    assert(limiter.allow("alice", t0));
    assert(limiter.allow("alice", t0));
    assert(!limiter.allow("alice", t0));
    assert(!limiter.allow("alice", t0 + 999ms));
    assert(limiter.allow("alice", t0 + 1s)); // t0 요청은 window 밖이다.

    assert(limiter.allow("bob", t0)); // key별 독립 bucket

    RateLimiter zero(0, 1s);
    assert(!zero.allow("key", t0));
}

void testSlidingWindowLogTimeReversal() {
    using namespace std::chrono_literals;

    const RateLimiter::TimePoint t0{};
    RateLimiter limiter(2, 1s);

    assert(limiter.allow("key", t0));
    assert(limiter.allow("key", t0 + 500ms));
    assert(!limiter.allow("key", t0 + 100ms)); // 500ms 시각으로 평가한다.
    assert(limiter.allow("key", t0 + 1s));     // t0 요청만 만료된다.
}

void testSlidingWindowLogConcurrentRequests() {
    constexpr int threadCount = 8;
    constexpr std::size_t limit = 5;

    const RateLimiter::TimePoint now{};
    RateLimiter limiter(limit, std::chrono::seconds(1));
    std::barrier startLine(threadCount);
    std::atomic<int> accepted = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&] {
            startLine.arrive_and_wait();
            if (limiter.allow("shared", now)) {
                ++accepted;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    assert(accepted == limit);
}

int main() {
    testLongestUniqueSubstring();
    testSlidingWindowLogBoundaries();
    testSlidingWindowLogTimeReversal();
    testSlidingWindowLogConcurrentRequests();
}
