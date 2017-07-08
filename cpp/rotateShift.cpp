#include <bitset>
#include <climits>
#include <iostream>

using namespace std;

using bit16 = short unsigned int;

bit16 rotateLeft(int n, int d) {
  bit16 leftMask = 1 << 15;

  bit16 ret = n;
  for (int i = 0; i < d; ++i) {
    bit16 leftBit = leftMask & ret;

    ret <<= 1;

    ret += leftBit ? 1 : 0;
  }

  return ret;
}

bit16 rotateRight(int n, int d) {
  bit16 leftMask = 1 << 15;

  bit16 ret = n;
  for (int i = 0; i < d; ++i) {
    bit16 rightBit = 1 & ret;

    ret >>= 1;

    ret += rightBit ? leftMask : 0;
  }

  return ret;
}

template <typename INT> INT rol(INT val, size_t len) {
  return (val << len) |
         ((unsigned)val >> (-len & (sizeof(INT) * CHAR_BIT - 1)));
}

template <typename INT> INT ror(INT val, size_t len) {
  return (val >> len) |
         ((unsigned)val << (-len & (sizeof(INT) * CHAR_BIT - 1)));
}

int main() {
  bit16 n = 229;
  int d = 3;
  cout << bitset<16>(n).to_string() << " :" << n << endl;

  bit16 l = rotateLeft(n, d);
  cout << bitset<16>(l).to_string() << " :(my-l) " << l << endl;

  bit16 r = rotateRight(n, d);
  cout << bitset<16>(r).to_string() << " :(my-r) " << r << endl;

  cout << bitset<16>(1875996).to_string() << " : " << 1875996 << endl;

  bit16 l2 = rol(229, 3);
  cout << bitset<16>(l2).to_string() << " :(ot-l) " << l2 << endl;

  bit16 r2 = ror(229, 3);
  cout << bitset<16>(r2).to_string() << " :(ot-r) " << r2 << endl;

  return 0;
}