#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

struct People {
  string name;
  int age;

  People(string n, int a) : name(n), age(a) {}

  void Dump() const { cout << name << "(" << age << ")" << endl; }
};

int main() {
  vector<shared_ptr<People>> v;

  // ("kim", 10), ("lee", 15), ("park", 5), ("jung", 20), ("choi", 12) 를 v에 넣어 주세요.

  v.insert(v.end(), {make_shared<People>("kim", 10), make_shared<People>("lee", 15), make_shared<People>("park", 5),
                     make_shared<People>("jung", 20), make_shared<People>("choi", 12)});

  // 이름순으로 정렬해서 출력해 보세요

  sort(begin(v), end(v), [](auto& a, auto& b) { return a->name < b->name; });

  for_each(begin(v), end(v), [](auto& n) { n->Dump(); });

  // 나이순으로 정렬해서 출력해 보세요.

  sort(begin(v), end(v), [](auto& a, auto& b) { return a->age < b->age; });

  for_each(begin(v), end(v), [](auto& n) { n->Dump(); });

  return 0;
}