// g++ -std=c++23 -pthread -Wall -Wextra day2.cpp -o a && ./a

#include <vector>
#include <iostream>
#include <cassert>
#include <numbers>
#include <utility>
#include <memory>
#include <string>

struct Node { int v; };
struct Node_raw { int val; Node_raw* next; };
struct Node_ptr { int val; std::unique_ptr<Node_ptr> next; };

int main() {

// **P1. unique_ptr 소유권 이전**
// - `struct Node { int v; };` 를 `make_unique`로 3개 만들어 `std::vector<std::unique_ptr<Node>>` 에 `std::move`로 담기.
// - move 이후 원본 포인터가 `nullptr`인지 확인 출력 → 벡터 순회하며 각 `v` 출력.

	auto a = std::make_unique<Node>(1);
	auto b = std::make_unique<Node>(2);
	auto c = std::make_unique<Node>(3);

	std::vector<std::unique_ptr<Node>> ret1;
	ret1.push_back(std::move(a));
	ret1.push_back(std::move(b));
	ret1.push_back(std::move(c));

	std::cout << a.get() << " " << b.get() << " " << c.get() << std::endl;
	for(const auto& x: ret1) { std::cout << x->v << " "; }
	std::cout << std::endl;

// **P2. shared_ptr 참조 카운트 추적**
// - `make_shared<int>(42)` 생성 후, **값으로** 받는 함수와 **참조로** 받는 함수를 각각 호출.
// - 생성 직후 / 값 전달 함수 안 / 함수 반환 후 각 지점에서 `use_count()` 출력 → 값 전달 시 올랐다 내려가는 것 확인.

	auto a1 = std::make_shared<int>(42);

	auto l1 = [](std::shared_ptr<int>& n) {
		std::cout << "in: " << *n << " use_count: " << n.use_count() << std::endl;
	};
	auto l2 = [](std::shared_ptr<int> n) {
		std::cout << "in: " << *n << " use_count: " << n.use_count() << std::endl;
	};

	l1(a1);
	std::cout << "out: " << *a1 << " use_count: " << a1.use_count() << std::endl;

	l2(a1);
	std::cout << "out: " << *a1 << " use_count: " << a1.use_count() << std::endl;

// P3. 복사 vs 이동 비용

// 원소 100만짜리 std::vector<std::string> 를 (a) 복사, (b) std::move로 다른 변수에 넘기기.
// move 이후 원본 .size()(빈 상태) 확인. 두 방식 차이를 메모에 한 줄 정리.

	std::vector<std::string> a2(1000000, "a");
	auto b2 = a2;
	auto c2 = std::move(a2);

	std::cout << a2.size() << std::endl;
	std::cout << b2.size() << std::endl;
	std::cout << c2.size() << std::endl;

	//memo
	//복사는 원소 모두를 b2에 복사하는 것이지만, std::move는 원소를 c2로 이동시키기에 a2에는 남지 않음.


// **(도전) P4. raw new/delete → 스마트 포인터 리팩터**
// - `new`로 잇는 단일 연결 리스트를 만든 뒤 `unique_ptr` 기반으로 리팩터해 `delete` 없이 누수 제거.

	auto end = new Node_raw{2, nullptr};
	auto start = new Node_raw{1, end};

	auto curr = start;

	while (curr != nullptr) {
		auto next = curr->next;
		std::cout << curr->val << " ";
		delete curr;
		curr = next;
	}
	std::cout << std::endl;

	// --- 내가 짠 버전 (동작은 하나 순회가 파괴적 = 1회용) ---
	// std::cout << "refactored" << std::endl;
	// auto end2 = std::make_unique<Node_ptr>();
	// end2->val = 2;
	// end2->next = nullptr;
	//
	// auto start2 = std::make_unique<Node_ptr>();
	// start2->val = 1;
	// start2->next = std::move(end2);
	//
	// auto curr2 = std::move(start2);
	// while (curr2 != nullptr) {
	// 	auto next = std::move(curr2->next);
	// 	std::cout << curr2->val << " ";
	// 	curr2 = std::move(next);
	// }
	// std::cout << std::endl;

	// --- 정답 BP: 생성은 make_unique(head 삽입), 순회는 non-owning raw ---
	std::cout << "refactored (BP)" << std::endl;

	// 생성: head-insertion. C++20 괄호 집계 초기화로 (val, next) 한 번에 넘김.
	std::unique_ptr<Node_ptr> head;
	head = std::make_unique<Node_ptr>(2, std::move(head));  // [2] -> null
	head = std::make_unique<Node_ptr>(1, std::move(head));  // [1] -> [2]

	// 순회: 소유권 건드리지 말고 raw 포인터(.get())로 "관찰만" → 리스트 보존
	for (Node_ptr* p = head.get(); p != nullptr; p = p->next.get())
		std::cout << p->val << " ";
	std::cout << std::endl;

	// delete 없음: head 소멸 시 next 체인이 연쇄 해제.
	// 주의: 아주 긴 리스트는 소멸자 재귀로 stack overflow 가능 → 실무는 std::list/forward_list.

	return 0;
}