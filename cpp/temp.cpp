#include <algorithm>
#include <climits>
#include <iostream>
#include <string>

using namespace std;

int firstSetBit(int n) {
  int ret = 1;
  while (n > 0) {
    if ((n & 1) == 1) {
      return ret;
    }

    n = n >> 1;
    ++ret;
  }

  return 0;
}

string determine(int n) {
  if (n == 0) {
    return "NO";
  }

  while (n > 0) {
    if ((n & 1) != 1) {
      return "NO";
    };

    n >>= 1;
  }

  return "YES";
}
int main() {
  cout << to_string(1 ^ 2) << endl;
  return 0;
}