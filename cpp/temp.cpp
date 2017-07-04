#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

auto t = [](auto a, auto b) { return a + b; };

int main() {
  int a = 1, b = 2;

  int ret = t(a, b);

  a = 3;
  b = 4;

  ret = t(a, b);

  double c, d;
  c = 3.5;
  d = 2.5;

  auto doubleTemp = t(c, d);

  cout << ret << endl;
}