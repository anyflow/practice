/*
INTERVIEW QUESTION — Thread-safe Circuit Breaker (40분)

연속 실패가 임계값에 도달하면 요청을 차단하고 cooldown 후 하나의 시험 요청만
허용하는 Circuit Breaker를 구현하라.

    enum class BreakerState { Closed, Open, HalfOpen };

    class CircuitBreaker {
    public:
        using TimePoint = std::chrono::steady_clock::time_point;

        CircuitBreaker(int failureThreshold,
                       std::chrono::milliseconds cooldown);
        bool allow(TimePoint now);
        void recordSuccess();
        void recordFailure(TimePoint now);
        BreakerState state() const;
    };

- Closed에서는 모든 요청을 허용하고 연속 실패가 임계값에 도달하면 Open이 된다.
- Open에서는 cooldown 전 요청을 거부한다.
- cooldown 후 첫 allow()만 HalfOpen 시험 요청을 선점하고 true를 반환한다.
- HalfOpen 시험 성공은 Closed, 실패는 다시 Open으로 전환한다.
- 허용된 요청은 결과를 정확히 한 번 기록하며 모든 함수는 thread-safe하다.

Open에서 도착한 결과는 Open 전 허용된 요청의 늦은 결과로 보고 무시한다.
Open → HalfOpen 전환은 별도 timer 없이 allow() 호출 시점에 lazy하게 수행한다.

FOLLOW-UP QUESTION & ANSWER

실패율 window 방식, 여러 프로세스의 상태 공유 여부, half-open herd 방지와 Envoy circuit breaking의 차이를 설명하라.

1. 실패율 window 방식
   현 구현은 연속 실패를 세지만, 실패율 방식은 최근 N개 또는 최근 T초의
   성공·실패를 저장하고 `failures / total >= threshold`이면 Open한다.
   표본이 적은 상태의 오판을 막기 위해 최소 요청 수 조건도 둔다.
   정확한 sliding log는 비싸므로 실전에서는 시간 bucket별 카운터를 많이 쓴다.

2. 여러 프로세스의 상태 공유
   현 구현의 상태는 프로세스 내부에만 있으므로 인스턴스별로 서로 다른 판단을
   할 수 있다. 보통은 조정 비용과 공유 저장소 장애의 전파를 피하려고 이를 허용한다.
   전체에서 하나의 상태가 반드시 필요하면 Redis 같은 외부 저장소와 분산 lock/lease가
   필요하지만, 네트워크 지연·일관성·가용성 문제가 생긴다.

3. HalfOpen herd 방지
   `allow()`가 mutex 안에서 Open → HalfOpen으로 전환하므로 첫 thread만
   `true`를 받고 나머지는 HalfOpen을 보고 거부된다. 단, 이 보장은 현 프로세스
   안에서만 유효하다. 전체 프로세스 중 하나만 시험해야 한다면 분산 lease가 필요하다.

4. Envoy circuit breaking과의 차이
   현 구현은 실패 결과에 따라 Closed/Open/HalfOpen을 전환하는 애플리케이션
   수준 circuit breaker다. Envoy가 `circuit_breakers`라고 부르는 기능은 upstream
   cluster의 최대 connection, pending request, active request, retry 수를 제한해
   overload를 빠르게 거부하는 자원 한도에 가깝다. Envoy에서 연속 5xx, 실패율 등으로
   문제 endpoint를 퇴출하는 기능은 `outlier_detection`이다. Envoy의 한 프로세스 내
   worker들은 한도를 공유하지만, 서로 다른 Envoy 프로세스끼리는 조정하지 않는다.
*/

#include <atomic>
#include <barrier>
#include <cassert>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

enum class BreakerState { Closed, Open, HalfOpen };

class CircuitBreaker {
public:
    using TimePoint = std::chrono::steady_clock::time_point;

private:
    const int failureThreshold_;
    const std::chrono::milliseconds cooldown_;

    BreakerState state_ = BreakerState::Closed;
    int consecutiveFailure_ = 0;
    TimePoint open_at_;

    mutable std::mutex mutex_;

  public:
    CircuitBreaker(int failureThreshold, std::chrono::milliseconds cooldown)
        : failureThreshold_(failureThreshold)
        , cooldown_(cooldown)
        {
            if(failureThreshold <= 0 || cooldown.count() <= 0) {
                throw std::invalid_argument("failureThreshold, cooldown must be positive");
            }
        }

    bool allow(TimePoint now) {
        std::lock_guard lock(mutex_);

        switch(state_) {
            case BreakerState::Closed:
                return true;

            case BreakerState::Open:
                if (now - open_at_ >= cooldown_) {
                    state_ = BreakerState::HalfOpen;
                    return true;
                }
                return false;

            case BreakerState::HalfOpen:
                return false;

            default:
                throw std::invalid_argument("out of range of BreakerState");
            }
    }

    void recordSuccess() {
        std::lock_guard lock(mutex_);

        switch(state_) {
            case BreakerState::Open:
                return;

            case BreakerState::Closed:
                consecutiveFailure_ = 0;
                return;

            case BreakerState::HalfOpen:
                state_ = BreakerState::Closed;
                consecutiveFailure_ = 0;
                return;

            default:
                throw std::invalid_argument("out of range of BreakerState");
        }
    }

    void recordFailure(TimePoint now) {
        std::lock_guard lock(mutex_);

        switch(state_) {
            case BreakerState::Closed:
                consecutiveFailure_ += 1;
                if(consecutiveFailure_ >= failureThreshold_) {
                    open_at_ = now;
                    state_ = BreakerState::Open;
                }
                return;

            case BreakerState::Open:
                return;

            case BreakerState::HalfOpen:
                open_at_ = now;
                state_ = BreakerState::Open;
                return;

            default:
                throw std::invalid_argument("out of range of BreakerState");
        }
    }

    BreakerState state() const {
        std::lock_guard lock(mutex_);

        return state_;
    }
};

void testThresholdCooldownAndRecovery() {
    using namespace std::chrono_literals;

    const CircuitBreaker::TimePoint t0{};
    CircuitBreaker breaker(2, 1s);

    bool allowed = breaker.allow(t0);
    assert(allowed);
    breaker.recordFailure(t0);
    assert(breaker.state() == BreakerState::Closed);

    allowed = breaker.allow(t0);
    assert(allowed);
    breaker.recordFailure(t0);
    assert(breaker.state() == BreakerState::Open);

    allowed = breaker.allow(t0 + 999ms);
    assert(!allowed);
    allowed = breaker.allow(t0 + 1s);
    assert(allowed);
    assert(breaker.state() == BreakerState::HalfOpen);

    allowed = breaker.allow(t0 + 1s);
    assert(!allowed);
    breaker.recordSuccess();
    assert(breaker.state() == BreakerState::Closed);
}

void testConsecutiveFailuresAndHalfOpenFailure() {
    using namespace std::chrono_literals;

    const CircuitBreaker::TimePoint t0{};
    CircuitBreaker breaker(2, 1s);

    bool allowed = breaker.allow(t0);
    assert(allowed);
    breaker.recordFailure(t0);

    allowed = breaker.allow(t0);
    assert(allowed);
    breaker.recordSuccess(); // 성공은 연속 실패 횟수를 초기화한다.

    allowed = breaker.allow(t0);
    assert(allowed);
    breaker.recordFailure(t0);
    assert(breaker.state() == BreakerState::Closed);

    allowed = breaker.allow(t0);
    assert(allowed);
    breaker.recordFailure(t0);
    assert(breaker.state() == BreakerState::Open);

    allowed = breaker.allow(t0 + 1s);
    assert(allowed);
    breaker.recordFailure(t0 + 1s);
    assert(breaker.state() == BreakerState::Open);

    allowed = breaker.allow(t0 + 1500ms);
    assert(!allowed);
    allowed = breaker.allow(t0 + 2s);
    assert(allowed);
    breaker.recordSuccess();
    assert(breaker.state() == BreakerState::Closed);
}

void testOnlyOneHalfOpenTrial() {
    using namespace std::chrono_literals;

    constexpr int threadCount = 8;
    const CircuitBreaker::TimePoint t0{};
    CircuitBreaker breaker(1, 1s);

    bool allowed = breaker.allow(t0);
    assert(allowed);
    breaker.recordFailure(t0);

    std::barrier startLine(threadCount);
    std::atomic<int> accepted = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&] {
            startLine.arrive_and_wait();
            if (breaker.allow(t0 + 1s)) {
                ++accepted;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    assert(accepted == 1);
    assert(breaker.state() == BreakerState::HalfOpen);
    breaker.recordSuccess();
    assert(breaker.state() == BreakerState::Closed);
}

int main() {
    testThresholdCooldownAndRecovery();
    testConsecutiveFailuresAndHalfOpenFailure();
    testOnlyOneHalfOpenTrial();
}
