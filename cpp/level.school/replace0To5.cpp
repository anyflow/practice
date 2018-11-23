// https://practice.geeksforgeeks.org/problems/number-of-occurrence/0

#include "../common/practice.h"

using namespace std;

int convertFive(int n) {
  int ret = 0;
  int placer = 1;

  while (n >= 1) {
    int digit = n % 10;

    if (digit == 0) {
      digit = 5;
    }

    ret += digit * placer;

    placer *= 10;

    n = (int)(n / 10);
  }

  return ret;
}

int main() {
  cout << convertFive(190104) << endl;

  return 0;
}