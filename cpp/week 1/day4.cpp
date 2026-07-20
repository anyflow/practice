// g++ -std=c++23 -pthread -Wall -Wextra day4.cpp -o a && ./a

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
#include <condition_variable>

using namespace std::chrono_literals;

void p1() {
// P1. 핑퐁 출력 (cv로 교대)**
// - 스레드 2개가 번갈아 1~10 출력: 결과가 `1 2 3 ... 10` 순서로 나오게.
// - 공유 `int turn` + cv로 자기 차례까지 `wait`.

	int turn = 1;
	bool ready1 = true, ready2 = false;
	std::mutex m;
	std::condition_variable cv;

	auto t1 = std::thread([&]() {
		std::unique_lock<std::mutex> ul(m);

		while(true) {
			cv.wait(ul, [&]{ return ready1; });

			std::cout << "t1: " << turn << " " << std::endl;

			++turn;
			ready1 = false;
			ready2 = true;
			cv.notify_one();

			if(turn >= 10) { return; }
		}
	});

	auto t2 = std::thread([&]() {
		std::unique_lock<std::mutex> ul(m);

		while(true) {
			cv.wait(ul, [&]{ return ready2; });

			std::cout << "t2: " << turn << " " << std::endl;

			++turn;
			ready2 = false;
			ready1 = true;
			cv.notify_one();

			if(turn >= 10) { return; }
		}
	});

	t1.join();
	t2.join();
}

void p2() {
// P2. 1칸짜리 bounded buffer (생산자-소비자)**
// - 버퍼 크기 1. 생산자가 값을 넣으면 소비자가 꺼내 출력, 소비자가 비워야 생산자가 다시 넣음.
// - 비었을 때/찼을 때 조건을 각각 `cv.wait(lk, pred)`로 대기. 총 20개 주고받고 종료.

	std::condition_variable cv;
	std::mutex m;
	std::vector<int> buffer(1);
	int count = 0;

	auto producer = std::thread([&]{
		while(true) {
			std::unique_lock<std::mutex> ul(m);
			cv.wait(ul, [&]{ return buffer[0] == 0; });

			buffer[0] = ++count;

			ul.unlock();
			cv.notify_one();

			if(count == 20) { return; }
		}
	});

	auto consumer = std::thread([&]{
		while(true) {
			std::unique_lock<std::mutex> ul(m);
			cv.wait(ul, [&]{ return buffer[0] != 0; });

			std::cout << buffer[0] << " ";
			buffer[0] = 0;

			ul.unlock();
			cv.notify_one();

			if(count == 20) { return; }
		}
	});

	producer.join();
	consumer.join();
}

void p3() {
// P3. 종료 신호 (notify_all)**
// - worker 3개가 `cv.wait(lk, []{ return start || stop; })` 로 대기.
// - 메인이 `stop = true; cv.notify_all();` 하면 셋 다 깨서 정상 종료. → `one`이 아니라 `all`이 필요한 이유 체감.

	std::mutex m;
	std::condition_variable cv;

	bool stop = false;

	std::vector<std::thread> ts;
	std::cout << std::endl;

	for(int i: {1,2,3}) {
		ts.emplace_back([&, i]{
			std::unique_lock<std::mutex> ul(m);
			cv.wait(ul, [&]{ return stop; });

			std::cout << i << " ";
		});
	}

	std::unique_lock<std::mutex> ul(m);
	stop = true;
	ul.unlock();

	cv.notify_all();

	for(auto& t: ts) { t.join(); }
}

int main() {

	p1();
	p2();
	p3();

	return 0;
}