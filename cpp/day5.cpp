// g++ -std=c++23 -pthread -Wall -Wextra day5.cpp -o a && ./a

#include <print>

#include <vector>
#include <iostream>
#include <cassert>
#include <numbers>
#include <utility>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <chrono>
#include <atomic>

#include <map>

#include <cstdlib>
#include <ctime>

using namespace std::chrono_literals;

void p1() {
// P1. atomic 카운터로 Day3-P1 재작성**
// - Day3의 8스레드 카운터 증가를 `std::atomic<int>` 로 다시 구현(락 없이 800,000). // 전역 `int counter = 0`. 스레드 8개가 각각 `counter`를 100,000번 증가.
// - mutex 버전과 코드/의미 차이를 `메모`에 한 줄.

	std::vector<std::thread> ts;
	std::atomic<int> counter{0};

	for(int i=0; i<8; ++i) {
		ts.emplace_back([&]{
			for(int i=0; i<100'000; ++i) {
				++counter;
			}
		});
	}

	for(auto& t: ts) { t.join(); }
	std::cout << counter.load() << std::endl;

// memo: atomic은 단일 카운터의 read-modify-write를 원자적으로 보장한다. 여러 변수나 컨테이너의 복합 불변식에는 mutex가 필요하다.
// 내부 원리는 동일하지 않음: mutex는 한 스레드만 임계영역에 들어가게 하고, 경쟁 시 스레드를 재울 수 있다.
// atomic은 보통 CPU의 원자적 명령으로 한 연산을 수행한다. 오히려 mutex가 내부 상태 관리에 atomic을 사용하는 경우가 많다.
}

void p2() {
// P2. shared_mutex 읽기 병렬 캐시**
// - `std::map<int,int>` 를 `shared_mutex`로 보호. `get`은 `shared_lock`, `set`은 `unique_lock`.
// - 읽기 스레드 8개(계속 조회) + 쓰기 스레드 1개(가끔 갱신)를 동시에 돌려 크래시/자료 깨짐 없이 동작 확인.
// - 왜 읽기 99%에서 유리한지 `메모`에 정리.
	std::shared_mutex m;
	std::map<int, int> buffer;

	std::vector<std::thread> ts;

	ts.emplace_back([&]{
		for(int i=0; i<100; ++i) {
			std::unique_lock<std::shared_mutex> ul(m);
			buffer[i] = i;
			ul.unlock();
		}
	});

	for(int i=0; i<8; ++i) {
		ts.emplace_back([&]{
			for(int i=0; i<100'000; ++i) {
				std::shared_lock<std::shared_mutex> ul(m);
				if(buffer.size() == 0) { continue; }

				int key = i % buffer.size();
				int val = buffer.at(key);

				assert(key == val);
				ul.unlock();
			}
		});
	}

	for(auto& t: ts) { t.join(); }

	assert(buffer.size() == 100);

// memo: shared_lock에서는 reader 간에 직렬화가 없어 overhead가 최소화되기 때문. 그 말은 99% 연산에서 lock overhead를 최소화할 수 있다는 뜻.
}

void p3() {
// P3. atomic 플래그로 정지**
// - `std::atomic<bool> running{true}` 로 루프 도는 worker. 메인이 `running = false` 하면 다음 반복에서 멈춤.
// - "이건 atomic으로 충분 / 이건 mutex 필요"의 경계를 한 줄 정리.

	std::atomic<bool> running{true};

	auto t = std::thread([&] {
		while(running.load()) {}
	});

	std::this_thread::sleep_for(10ms);
	running.store(false);

	t.join();

// memo: atomic의 단일 연산으로 불변식을 만족하면 atomic, 아니면 mutex
}

int main() {

	p1();
	p2();
	p3();

	return 0;
}