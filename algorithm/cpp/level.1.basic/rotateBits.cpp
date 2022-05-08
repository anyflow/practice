// https://practice.geeksforgeeks.org/problems/rotate-bits/0

#include "../lib/practice.h"

using namespace std;

using bit16 = short unsigned int;

void printRotate(bit16 N, int D) {
  bit16 left, right, leftBit, rightBit;

  bit16 leftMask = 1 << 15;
  bit16 rightMask = 1;

  left = right = N;

  for (int i = 0; i < D; ++i) {
    leftBit = leftMask & left;
    left <<= 1;

    left += leftBit ? 1 : 0;

    rightBit = rightMask & right;
    right >>= 1;

    right += rightBit ? leftMask : 0;
  }

  cout << to_string(left) << endl << to_string(right) << endl;
}

int main() {
  int T;

  cin >> T;
  while (T--) {
    bit16 N;
    int D;

    cin >> N >> D;

    printRotate(N, D);
  }
}