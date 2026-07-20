// g++ -std=c++23 -pthread -Wall -Wextra day3.cpp -o a && ./a

#include <vector>
#include <iostream>
#include <cassert>
#include <numbers>
#include <utility>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <print>
#include <chrono>

using namespace std::chrono_literals;

void race_condition_work(int& counter) {
	for(int i=0; i<100'000; ++i) {
		++counter;
	}
}

void mutex_work(int& counter, std::mutex& m) {
	for(int i=0; i<100'000; ++i) {
		{
			std::scoped_lock sl(m);
			++counter;
		}
	}
}

struct Account { std::mutex m; int balance; };


void p1() {
// P1. race condition 재현 → mutex로 수정**
// - 전역 `int counter = 0`. 스레드 8개가 각각 `counter`를 100,000번 증가.

// - (a) 락 없이 → 최종값이 800,000보다 작게 나오는 것(유실) 확인.
	int counter = 0;
	std::vector<std::thread> ts;

	for(int i=0; i<8; ++i) {
		ts.push_back(std::thread([&counter] { race_condition_work(counter); }));
	}
	for(auto& t: ts) {
		t.join();
	}

	std::cout << counter << std::endl;

	counter = 0;
	ts.clear();

// - (b) `std::mutex` + `lock_guard`로 감싸 정확히 800,000 나오게 수정.
	std::mutex m;
	for(int i=0; i<8; ++i) {
		ts.push_back(std::thread([&counter, &m] { mutex_work(counter, m); }));
	}
	for(auto& t: ts) {
		t.join();
	}

	std::cout << counter << std::endl;
}

void p2() {
// **P2. 병렬 부분합 (공유 없이)**
// - 크기 100만 `vector<int>` 를 스레드 N개로 등분, 각 스레드가 자기 구간 합을 **자기 인덱스**의 결과 배열에 저장(공유 쓰기 없음).
// - join 후 부분합을 모아 총합 출력. → "락이 필요 없게 설계하는" 예.

	std::vector<int> v(1000'000);
	for(size_t i=0; i<v.size(); ++i) { v[i] = i; }

	int N = 3;
	size_t t_size = (size_t)(v.size() / N);
	std::cerr << t_size << "\n";

	std::vector<std::vector<int>> partials;
	std::vector<int>* partial = nullptr;
	for(size_t i=0; i<v.size(); ++i) {
		if(i % t_size == 0) {
			partial = &partials.emplace_back();
		}
		partial->push_back(i);
	}

	std::vector<long long> sums(partials.size());
	std::vector<std::thread> ts;
	size_t index = 0;
	for(auto& partial: partials) {
		ts.emplace_back(std::thread([partial, &sums, index]{
			long long sum = 0;
			for(auto& i: partial) { sum += i; }

			sums[index] = sum;
		}));
		++index;
	}
	for(auto& t: ts) { t.join(); }

	long long ret = 0;
	for(const long long sum: sums) { ret += sum; }

	std::cout << ret << std::endl;
}

void p3() {
// **P3. deadlock 재현 → 해결**
// - 계좌 두 개(각자 `mutex`) 사이 이체 `transfer(a, b)`.
// - 두 스레드가 `transfer(A,B)`, `transfer(B,A)`를 동시에 → 락 순서 엇갈려 **deadlock 재현**.
// - `std::scoped_lock lk(mA, mB)` 로 해결.

	Account a{{}, 100}, b{{}, 200};

	auto transfer = [](Account& from, Account& to) {
		// std::lock_guard<std::mutex> lma(from.m);
		// std::this_thread::sleep_for(10ms);
		// std::lock_guard<std::mutex> lmb(to.m);
		std::scoped_lock lock(from.m, to.m);

		to.balance += from.balance;
		from.balance = 0;
	};

	auto t1 = std::thread(transfer, std::ref(a), std::ref(b));
	auto t2 = std::thread(transfer, std::ref(b), std::ref(a));

	t1.join();
	t2.join();

	std::cout << a.balance << " " << b.balance << std::endl;
}

int main() {

	p1();
	p2();
	p3();

	return 0;
}