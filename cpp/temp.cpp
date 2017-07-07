#include <algorithm>
#include <climits>
#include <iostream>
#include <string>

using namespace std;

int getFirstSet(int k) {
  if (k == 0) {
    return 0;
  }
  if (k == 1) {
    return 1;
  }

  int ret = 1;
  while (k > 1) {
    int rest = k % 2;
    if (rest == 1) {
      return ret;
    }

    k /= 2;
    ++ret;
  }
}

int main() {
  int* n = nullptr;
  int* m = nullptr;

  return 0;
}