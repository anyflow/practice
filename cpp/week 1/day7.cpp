// g++ -std=c++23 -pthread -Wall -fsanitize=address,undefined -fno-omit-frame-pointer -Wextra day7.cpp -o a && ./a

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
#include <unordered_map>
#include <vector>
#include <deque>
#include <queue>

#include <cstdlib>
#include <ctime>

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <optional>

#include <stdexcept>

#include <future>

using namespace std::chrono_literals;

template<typename T>
class TSQueue {
	std::mutex m;
	std::condition_variable cv;
	std::queue<T> q;
	bool done;
// - `template<class T> class TSQueue` 요구사항:

public:
	explicit TSQueue(): done(false) {}

	void push(T value) {
//   - `push(T)` — 넣고 대기자 하나 깨움
		{
			std::unique_lock<std::mutex> ul(m);
			if(done) { throw std::runtime_error("already done!"); }

			q.push(std::move(value));
		}
		cv.notify_one();
	}

	std::optional<T> pop() {
//   - `pop()` — 비면 블로킹 대기, 값 반환
		std::unique_lock<std::mutex> ul(m);
		cv.wait(ul, [&] { return !q.empty() || done; });

		if(q.empty()) { return std::nullopt; }

		T ret = std::move(q.front());
		q.pop();
		return ret;
	}

	std::optional<T> try_pop() {
//   - `try_pop(T&)` — 비면 즉시 `false`, 있으면 꺼내고 `true`
		std::unique_lock<std::mutex> ul(m);
		if(q.empty()) { return std::nullopt; }

		T ret = std::move(q.front());
		q.pop();
		return ret;

	}

	void shutdown() {
//   - `shutdown()` — 이후 대기 중인 `pop`은 모두 깨어나 빠져나감 (빈 `optional`/`false` 반환)
		std::unique_lock<std::mutex> ul(m);
		done = true;
		ul.unlock();

		cv.notify_all();
	}

	size_t size() {
		std::unique_lock<std::mutex> ul(m);
		return q.size();
	}
};

void p1() {
// P1. Thread-safe Queue (백지, 종료 지원)**
// - 검증: 생산자 3 / 소비자 2로 총 300개 처리 후 `shutdown`으로 전 스레드 정상 종료.
// - **제약: 참고 없이 15분 안.** 짜면서 5단계(자료구조 / 락 / cv / 종료 / trade-off) 소리 내어 설명.

	TSQueue<int> queue;

	std::vector<std::thread> producers;
	for(int i = 0; i < 3; ++i) {
		int start = i * 100;
		producers.emplace_back([start, &queue] {
			for(int j = start; j < start + 100; ++j) {
				queue.push(j);
			}
		});
	}

	std::vector<std::vector<int>> consumes(2);
	std::vector<std::thread> consumers;
	for(int i = 0; i < 2; ++i) {
		consumers.emplace_back([&, i] {
			while(auto item = queue.pop()) {
				consumes[i].push_back(*item);
			}
		});
	}

	for(auto& t: producers) { t.join(); }
	queue.shutdown();

	for(auto& t: consumers) { t.join(); }

	// std::cout << consumes[0].size() + consumes[1].size() << std::endl;
	assert(consumes[0].size() + consumes[1].size() == 300);
	assert(queue.size() == 0);

	auto item = queue.try_pop();
	assert(item == std::nullopt);

	TSQueue<int> queue1;
	queue1.push(1);
	item = queue1.try_pop();
	assert(item.value_or(2) == 1);
	queue1.shutdown();
}

template<typename K, typename V>
class KVStore {
	std::shared_mutex m;
	std::unordered_map<K, V> data;

public:
	explicit KVStore() {}

	bool get(const K& key, V& out) {
		std::shared_lock<std::shared_mutex> sl(m);

		if(auto it = data.find(key); it != data.end()) {
			out = it->second;
			return true;
		}
		else {
			return false;
		}
	}

	void put(const K& key, V out) {
		std::unique_lock<std::shared_mutex> ul(m);

		data[key] = std::move(out); // replace if exist
	}

	bool erase(const K& key) {
		std::unique_lock<std::shared_mutex> ul(m);
		return data.erase(key) == 1;
	}

	size_t size() {
		std::shared_lock<std::shared_mutex> sl(m);
		return data.size();
	}
};

void p2() {
// **P2. Thread-safe KV Store**
// - `template<class K, class V> class KVStore` 요구사항:
//   - `bool get(const K&, V& out)` — 있으면 `out` 채우고 `true`
//   - `void put(const K&, V)` / `bool erase(const K&)` / `size_t size()`
// - `unordered_map`을 `std::shared_mutex`로 보호한다. `get`과 `size`는 `shared_lock`, `put`과 `erase`는 `unique_lock`을 사용한다.
// - 검증:
//   1. 시작 전에 key `0~99`를 value `key * 10`으로 저장한다.
//   2. reader 8개는 key `0~99`를 각각 1,000회 읽으며 반환값과 value를 검사한다.
//   3. writer 2개는 reader와 동시에 실행하며 서로 겹치지 않는 key `100~1,099`, `1,100~2,099`를 value `key * 10`으로 저장한다.
//   4. 모든 thread 종료 후 `size() == 2,100`과 key `0~2,099`의 value를 검사한다.
//   5. key 하나를 `erase()`해 첫 호출은 `true`, 같은 key의 두 번째 호출은 `false`, 최종 `size() == 2,099`인지 검사한다.
// - follow-up 답 준비: "읽기 99%면?", "쓰기 많아지면?", "규모 커지면(샤딩)?", "Redis 쓰면?"

	KVStore<int, int> data;
	for(int i=0; i<100; ++i) { data.put(i, i * 10); }

	std::vector<std::thread> readers;
	for(int i=0; i<8; ++i) {
		readers.emplace_back([&] {
			for(int j=0; j<1'000; ++j) {
				for(int i=0; i<100; ++i) {
					int val = -1;
					bool ret = data.get(i, val);

					assert(ret);
					assert(val == i * 10);
				}
			}
		});
	}

	std::vector<std::thread> writers;
	for(int i=0; i<2; ++i) {
		writers.emplace_back([&, i] {
			int start = 100 + i * 1000;
			for(int j=start; j<start + 1000; ++j) {
				data.put(j, j * 10);
			}
		});
	}

	for(auto& t: readers) { t.join(); }
	for(auto& t: writers) { t.join(); }

	// std::cerr << data.size() << std::endl;

	assert(data.size() == 2'100);
	for(size_t i=0; i<data.size(); ++i) {
		int val = -1;
		bool ret = data.get((int)i, val);

		assert(ret);
		assert((size_t)val == i * 10);
	}

	bool erased = data.erase(0);
	assert(erased == true);

	erased = data.erase(0);
	assert(erased == false);
	assert(data.size() == 2'099);

// • - 읽기 99%면?
//     shared_mutex를 사용해 get/size는 shared_lock, 쓰기는 unique_lock으로 처리한다. Reader 병렬성이 실제 성능 향상으로 이어지는지는
//     측정한다.

//   - 쓰기가 많아지면?
//     Writer는 어차피 독점 lock이 필요하고 shared_mutex 자체의 비용도 있으므로 일반 mutex가 더 나을 수 있다. Writer starvation 가능
//     성도 확인한다.

//   - 규모가 커지면?
//     hash(key) % N으로 데이터를 여러 shard로 나누고 shard마다 map과 mutex를 둔다. 서로 다른 shard는 병렬 처리되지만 전체 size()와
//     snapshot은 여러 lock을 다뤄야 한다.

//   - Redis를 사용하면?
//     process 내부 map과 mutex 대신 외부 Redis가 동시 접근과 저장을 담당한다. 여러 서버가 데이터를 공유하고 TTL·복제·장애 복구를 사
//     용할 수 있지만, network 지연·직렬화·Redis 장애·일관성 정책을 추가로 처리해야 한다.
}

int main() {

	p1();
	p2();

	return 0;
}