#include <bitset>
#include <iostream>
using namespace std;

int main() {
  unsigned t = 5, bit = 0;

  unsigned ret = 0;

  for (int i = 1; i < 32; ++i) {
    ret |= t & 1;

    t >>= 1;
    ret <<= 1;
  }

  cout << ret << endl;

  cout << bitset<32>(ret).to_string() << endl;
  cout << bitset<4>("1010").to_ulong() << endl;
  return 0;
}