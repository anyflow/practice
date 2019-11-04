#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

class Person {
  std::string name__;

public:
  Person(std::string s) : name__(s) {}

  Person(const Person& p) : name__(p.name__) {
    std::cout << "copy: " << this->name__ << std::endl;
  }

  Person(const Person&& p) noexcept : name__(std::move(p.name__)) {
    std::cout << "move: " << this->name__ << std::endl;
  }

  Person& operator=(const Person& p) {
    name__ = p.name__;
    std::cout << "copy= : " << this->name__ << std::endl;
    return *this;
  }

  Person& operator=(const Person&& p) noexcept {
    name__ = std::move(p.name__);
    std::cout << "move= : " << this->name__ << std::endl;
    return *this;
  }

  std::string name() { return this->name__; }
};

int main() {
  using namespace std;

  vector<Person> v;

  v.push_back(Person("A"));
  v.push_back(Person("B"));
  v.push_back(Person("C"));
  v.push_back(Person("D"));

  cout << "-----------------------" << endl;

  auto p1 = begin(v);

  Person peo1 = *p1;

  // move_iterator<vector<Person>::iterator> p2;
  auto p2 = make_move_iterator(p1);
  Person peo2 = *p2;

  cout << "size of v : " << v.size() << endl;
  cout << "begin of v : " << (*(v.begin())).name() << endl;
}