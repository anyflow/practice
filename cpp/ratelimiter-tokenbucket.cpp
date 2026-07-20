/*
INTERVIEW QUESTION — Keyed Token Bucket Rate Limiter (40분)

한 프로세스에서 여러 thread가 공유하는 key별 Token Bucket을 구현하라.
테스트가 실제 시간이나 sleep에 의존하지 않도록 요청 시각을 인자로 받는다.

제공할 API

    class RateLimiter {
    public:
        using TimePoint = std::chrono::steady_clock::time_point;

        RateLimiter(std::size_t capacity, double tokensPerSecond);
        bool allow(const std::string& key, TimePoint now);
        std::size_t keyCount() const;
    };

동작 계약

- 각 key는 독립된 bucket을 가지며 처음에는 capacity만큼 차 있다.
- token은 경과 시간에 비례해 연속 보충되며 capacity를 넘지 않는다.
- token이 하나 이상이면 하나를 소비하고 true, 아니면 false를 반환한다.
- 같은 key의 refill과 소비는 하나의 원자적 결정이어야 한다.
- capacity 0은 모든 요청을 거부하고 tokensPerSecond 0은 보충하지 않는다.
- 같은 key에 전달되는 시간이 역행해도 token을 추가하거나 기준 시각을 되돌리지 않는다.

제출 전 검증

- 초기 burst, 소진, 부분·전체 refill과 capacity 상한
- 서로 다른 key의 독립성, capacity 0, 시간 역행
- 같은 시각의 동시 요청에서 정확히 capacity개만 허용되는지 확인
- sleep 없는 테스트와 ThreadSanitizer 실행

Follow-up

1. Token Bucket이 burst를 허용하면서 평균 속도를 제한하는 원리는 무엇인가?
   답: 사용하지 않은 token을 capacity까지 저장해 순간 burst에 쓰되 장기적으로는 refill rate만큼만 보충한다.
2. 호출자가 락 밖에서 now를 얻으면 시간 역행이 어떻게 발생할 수 있는가?
   답: 먼저 시각을 얻은 thread가 늦게 락을 획득하면 이미 처리된 더 최신 시각 뒤에 과거 시각이 들어올 수 있다.
3. key 수가 계속 증가할 때 오래된 bucket을 어떻게 정리할 것인가?
   답: 마지막 접근 시각을 저장하고 TTL이 지난 bucket을 접근 시 lazy 삭제하거나 주기적인 cleanup 작업으로 제거한다.
4. 하나의 mutex가 병목이면 sharding 또는 key별 lock을 어떻게 적용하는가?
   답: key hash로 고정된 shard를 선택해 shard별 mutex를 두거나 bucket별 mutex와 안전한 수명 관리를 적용한다.
5. 여러 프로세스로 확장할 때 Redis와 원자적 script가 필요한 이유는 무엇인가?
   답: 남은 token이 1개일 때 두 프로세스가 동시에 1을 읽고 둘 다 허용할 수 있으므로 Redis script로 refill·검사·소비를 원자적으로 수행한다.
6. 장애 시 fail-open과 fail-closed 중 무엇을 선택할 것인가?
   답: limiter 장애 시 요청을 허용하는 fail-open은 가용성 우선에, 요청을 거부하는 fail-closed는 보안·과금·자원 보호 우선에 선택한다.
*/



// ASan/UBSan:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     RateLimiter.cpp -o limiter && ./limiter
//
// ThreadSanitizer:
// g++ -std=c++23 -pthread -Wall -Wextra -Wpedantic \
//     -fsanitize=thread RateLimiter.cpp -o limiter-tsan && ./limiter-tsan

#include <cstddef>
#include <chrono>
#include <string>
#include <mutex>
#include <cassert>
#include <vector>
#include <barrier>
#include <unordered_map>
#include <utility>
#include <thread>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <limits>

class RateLimiter {
  public:
    using TimePoint = std::chrono::steady_clock::time_point;

  private:
    using Bucket = std::pair<double, TimePoint>; //key, <tokens, last_refill>

    const double capacity_;
    const double tokens_per_second_;

    std::unordered_map<std::string, Bucket> buckets_;

    mutable std::mutex mutex_;

  public:
    RateLimiter(std::size_t capacity, double tokensPerSecond)
        : capacity_(static_cast<double>(capacity))
        , tokens_per_second_(tokensPerSecond)
        {
            if(!std::isfinite(tokens_per_second_) || tokens_per_second_ < 0.0) {
                throw std::invalid_argument("tokensPerSecond must be finite and non-negative");
            }
        }

    bool allow(const std::string& key, TimePoint now) {
        if(capacity_ == 0.0) {
            return false;
        }

        std::lock_guard lock(mutex_);

        auto found = buckets_.find(key);
        if(found == buckets_.end()) {
            buckets_.try_emplace(key, capacity_ - 1, now);
            return true;
        }

        auto& [tokens, last_refill] = found->second;

        if(now > last_refill) {
            auto elapsed = std::chrono::duration<double>(now - last_refill).count();
            tokens = std::min(tokens + tokens_per_second_* elapsed, capacity_);
            last_refill = now;
        }

        if(tokens < 1.0) {
            return false;
        }

        tokens -= 1.0;
        return true;
    }

    std::size_t keyCount() const {
        std::lock_guard lock(mutex_);

        return buckets_.size();
    }
};

void testRefillAndCapacity() {
    using namespace std::chrono_literals;

    const RateLimiter::TimePoint t0{};
    RateLimiter limiter(5, 2.0);

    for (int i = 0; i < 5; ++i) {
        assert(limiter.allow("alice", t0));
    }
    assert(!limiter.allow("alice", t0));
    assert(!limiter.allow("alice", t0 + 250ms)); // 0.5 token
    assert(limiter.allow("alice", t0 + 500ms));  // 1 token refill 후 소비

    const auto later = t0 + 10s;
    for (int i = 0; i < 5; ++i) {
        assert(limiter.allow("alice", later));
    }
    assert(!limiter.allow("alice", later)); // capacity를 넘게 보충하지 않는다.
}

void testIndependentKeysAndBoundaries() {
    using namespace std::chrono_literals;

    const RateLimiter::TimePoint t0{};
    RateLimiter limiter(1, 1.0);

    assert(limiter.allow("alice", t0));
    assert(!limiter.allow("alice", t0));
    assert(limiter.allow("bob", t0));
    assert(limiter.keyCount() == 2);

    assert(limiter.allow("alice", t0 + 1s));
    assert(!limiter.allow("alice", t0));      // 역행한 시간은 무시한다.
    assert(limiter.allow("alice", t0 + 2s)); // 기준 시각도 되돌아가지 않았다.

    RateLimiter zeroCapacity(0, 10.0);
    assert(!zeroCapacity.allow("key", t0));

    RateLimiter noRefill(1, 0.0);
    assert(noRefill.allow("key", t0));
    assert(!noRefill.allow("key", t0 + 1h));

    for (const double invalidRate : {
             -1.0,
             std::numeric_limits<double>::infinity(),
             -std::numeric_limits<double>::infinity(),
             std::numeric_limits<double>::quiet_NaN()}) {
        bool rejected = false;
        try {
            RateLimiter invalid(1, invalidRate);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        assert(rejected);
    }
}

void testConcurrentRequests() {
    constexpr int threadCount = 8;
    constexpr int attemptsPerThread = 1'000;

    const RateLimiter::TimePoint now{};
    RateLimiter limiter(5, 2.0);
    std::barrier startLine(threadCount);
    std::atomic<int> accepted = 0;
    std::vector<std::thread> threads;

    for (int id = 0; id < threadCount; ++id) {
        threads.emplace_back([&] {
            startLine.arrive_and_wait();
            for (int attempt = 0; attempt < attemptsPerThread; ++attempt) {
                if (limiter.allow("shared", now)) {
                    ++accepted;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    assert(accepted == 5);
}

int main() {
    testRefillAndCapacity();
    testIndependentKeysAndBoundaries();
    testConcurrentRequests();
}
