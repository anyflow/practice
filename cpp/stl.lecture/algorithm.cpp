#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

#include "./library.hpp"

using namespace std;

int main() {
  list<int> s1 = {1, 2, 3, 4, 5};
  list<int> s2 = {1, 2, 3, 4, 5};
  list<int> s3;

  // transform(s1.begin(), s1.end(), back_inserter(s3),
  //           [](int a) { return a * 2; });

  // show(s3);

  // transform(s1.begin(), s1.end(), s2.begin(), s3.begin(),
  //           [](int a, int b) { return a * b; });

  // show(s3);

  // auto p = find_if(s1.begin(), s1.end(), [](int n) { return n % 3 == 0; });

  // cout << *p << endl;

  // s3.clear();
  auto p = remove_copy_if(s1.begin(), s1.end(), back_inserter(s3),
                          [](int n) { return n % 3 == 0; });

  show(s3);

  // cout << *prev(p, 2) << endl;

  // remove_if(s1.begin(), s1.end(), [](int n) { return n % 3 == 0; });

  // show(s1);

  return 0;
}