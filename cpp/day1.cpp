// g++ -std=c++23 -pthread -Wall -Wextra day1.cpp -o a && ./a

#include <vector>
#include <iostream>
#include <cassert>
#include <numbers>
#include <utility>

double pi = std::numbers::pi;
using std::vector;
using std::cout;
using std::endl;
using std::unreachable;

vector<int> double_evens(const vector<int>& v) {
	auto is_even = [](int x){ return x % 2 == 0; };
	vector<int> ret;

	for(const auto& x: v) {
		if(is_even(x)) {
			ret.push_back(x * 2);
		}
	}

	return ret;
}

enum class Op { Add, Sub, Mul };

int cal(int x, int y, Op op) {
	switch(op) {
		case Op::Add:
			return x + y;
		case Op::Sub:
			return x - y;
		case Op::Mul:
			return x * y;
		default:
			unreachable();
	}
}

struct Shape {
	virtual ~Shape() = default;
	virtual double area() = 0;
};

struct Circle : Shape {
	int r;
	Circle(int r): r(r) {}

	double area() override {
		return pi * r * r;
	}
};

struct Rect : Shape {
	int x;
	int y;
	Rect(int x, int y): x(x), y(y) {}

	double area() override {
		return x * y;
	}
};

int main() {
    vector<int> v = {1,2,3,4,5,6};

	// P1
	for(const auto& x: double_evens(v)) {
		cout << x << " ";
	}

	// **P2. 람다 캡처 두 방식 (`[&]` vs `[=]`)**
	// - 정수 벡터를 받아 (a) `[&]`로 바깥 `sum`에 합계를 누적하는 람다, (b) 바깥 `threshold`를 `[=]`로 캡처해 그 이상인 원소 개수를 세는 람다를 각각 만들어 실행.
	// - 실행 후 `sum` 값과 두 캡처의 차이를 눈으로 확인.

	int sum = 0;
	int count = 0;
	int threshold = 3;

	auto accu = [&sum](int x){ sum += x; };
	auto more = [threshold, &count](int x){
		if(x >= threshold) {
			++count;
		}
	};

	for(const auto& x: v) {
		accu(x);
		more(x);
	}

	cout << sum << " " << count << endl;

	// **P3. enum class 계산기 + override**
	// - (a) `enum class Op { Add, Sub, Mul }` 와 두 수를 받아 결과를 반환하는 함수(switch).
	// - (b) `virtual double area()` 를 갖는 `Shape` 기반 `Circle`/`Rect`(각자 `override`). `Shape*` 여러 개를 벡터에 담아 range-for로 순회하며 면적 합 출력.

	cout << cal(3, 4, Op::Add) << " " << cal(3, 4, Op::Sub) << " " << cal(3, 4, Op::Mul) << endl;

	vector<Shape*> shapes = { new Circle(3), new Circle(4), new Rect(3, 4), new Rect(4, 5) };

	double sum2 = 0;
	for(auto s: shapes) {
		sum2 += s->area();
		delete s;
	}

	cout << sum2 << endl;
}
