// g++ -std=c++23 -pthread -Wall -Wextra day6.cpp -o a && ./a

#include <print>


#include <iostream>
#include <cassert>
#include <numbers>
#include <utility>
#include <memory>
#include <string>

#include <chrono>
#include <atomic>

#include <map>
#include <vector>
#include <deque>

#include <cstdlib>
#include <ctime>

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <stdexcept>

#include <future>

using namespace std::chrono_literals;

void p1() {
// **P1. async 병렬 부분합**
// - 크기 100만 `vector<int>` 를 반으로 나눠 각 절반 합을 `std::async(std::launch::async, ...)` 로 계산, 두 `future.get()` 을 더해 총합 출력.
	std::cout << "\n\nP1\n";

	size_t size = 1'000'000;
	std::vector<int> items(size);
	long long answer = 0;
	for(size_t i=0; i<size; ++i) {
		items[i] = i;
		answer += i;
	}

	std::future<long long> f1 = std::async(std::launch::async, [&items]{
		auto end = items.size() / 2;
		long long sum = 0;
		for(size_t i=0; i<end; ++i) { sum += items[i]; }
		return sum;
	});

	std::future<long long> f2 = std::async(std::launch::async, [&items]{
		auto start = items.size() / 2;
		long long sum = 0;
		for(size_t i=start; i<items.size(); ++i) { sum += items[i]; }
		return sum;
	});

	auto sum = f1.get() + f2.get();
	std::cout << sum << std::endl;

	assert(answer == sum);
}

void p2() {
// **P2. promise/future로 결과 + 예외 전달**
// - 워커 스레드가 `promise`에 결과를 `set_value`, 메인이 `future.get()`으로 수신.
// - 워커가 조건에 따라 `throw` 하게 만들고, `get()`에서 `try/catch`로 잡히는지 확인.
	std::cout << "\n\nP2\n";

	bool condition = false;
	int value = 0;
	std::promise<int> p1;
	auto proc = [&] {
		try {
			if(condition) { p1.set_value(value); }
			else { throw std::runtime_error("threw"); }
		}
		catch(...) {
			p1.set_exception(std::current_exception());
		}
	};

	std::thread worker1(proc);

	try {
		int result = p1.get_future().get();
		assert(value == result);
	}
	catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	worker1.join();
}

void p3() {
// **P3. launch policy 차이 관찰**
// - 같은 작업을 `std::launch::async`와 `std::launch::deferred`로 각각 실행한다.
// - main과 작업 내부의 thread ID, `get()` 전 `wait_for(0s)` 상태, `get()` 전후 로그를 비교한다.
// - thread ID는 `std::this_thread::get_id()`로 얻을 수 있다.
// - 로그 순서만으로 판단하지 말고 두 정책의 실행 thread와 실행 시점 차이를 설명한다.
	std::cout << "\n\nP3\n";

	std::cout << "main: " << std::this_thread::get_id() << '\n';
	auto work = []{
		std::cout << "worker: " << std::this_thread::get_id() << '\n';
		return 1;
	};

	std::future<int> f_async = std::async(std::launch::async, work);

	auto status = f_async.wait_for(0s);
	std::cout
		<< (status == std::future_status::ready    ? "ready" :
			status == std::future_status::timeout  ? "timeout" :
													"deferred")
		<< '\n';

	f_async.get();

	std::future<int> f_deferred = std::async(std::launch::deferred, work);

	status = f_deferred.wait_for(0s);
	std::cout
		<< (status == std::future_status::ready    ? "ready" :
			status == std::future_status::timeout  ? "timeout" :
													"deferred")
		<< '\n';

	f_deferred.get();
}

template<typename T>
class TSQueue {
	std::mutex mutex;
	std::condition_variable cv;
	std::deque<T> queue;

public:
	explicit TSQueue() {}

	void push(T value) {
		std::unique_lock<std::mutex> ul(mutex);
		queue.push_back(std::move(value));
		ul.unlock();

		cv.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> sl(mutex);
		cv.wait(sl, [&]{ return !queue.empty(); });

		T ret = queue.front();
		queue.pop_front();
		return ret;
	}
};


void p4() {
	std::cout << "\n\nP4\n";

// P4. Thread-safe Queue v1**
// - `template<class T> class TSQueue` 에 `push(T)` + 블로킹 `pop()` 구현 (`mutex` + `condition_variable`).
// - `push`는 넣고 `notify_one`, `pop`은 `cv.wait(lk, [&]{ return !q.empty(); })` 후 꺼냄.
// - 생산자 1 / 소비자 1로 100개 주고받아 동작 확인. (종료 처리는 Day7에서 추가)
	std::cout << "\n\nP4\n";

	TSQueue<int> queue;

	std::thread producer([&] {
		for(int i=0; i<100; ++i) {
			std::cout << "producer: " << i << "\n";
			queue.push(i);
		}
	});

	std::thread consumer([&] {
		for(int i=0; i<100; ++i) {
			int value = queue.pop();
			std::cout << "consumer: " << value << "\n";
			assert(value == i);
		}
	});

	producer.join();
	consumer.join();
}
int main() {

	p1();
	p2();
	p3();
	p4();

	return 0;
}