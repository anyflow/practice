#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>

using namespace std;

template <typename C> class eback_insert_iterator {
  C* container;

public:
  eback_insert_iterator(C& c) : container(&c) {}

  eback_insert_iterator& operator*() { return *this; }

  eback_insert_iterator& operator=(const typename C::value_type& a) {
    container->push_back(a);
    return *this;
  }
};

int main() {
  list<int> list1 = {1, 2, 3};

  auto i = begin(list1);

  int n = *i; // input iterator

  *i = 20; // output iterator

  ++i; // ok

  // i = i + 2; // error (but OK in vector - because + operator is defined
  // semantically in vector, deque)

  cout << *i << endl;

  // multipass guarantee definition

  auto i1 = begin(list1);
  auto i2 = i1;

  if (i1 == i2) {
    cout << (*i1 == *i2) << endl;
    cout << (++i1 == ++i2) << endl;
  }

  // stream iterator, insert iterator are not guarantee multipass!!

  list<int> s = {1, 2};

  eback_insert_iterator p(s);

  p = 20;
  *p = 30;

  for (auto& n : s)
    cout << n << ", ";

  cout << endl;

  cout << "size : " << s.size() << endl;

  list<int>::
}