#include <bitset>
#include <iostream>
#include <string>

using namespace std;

int main() {
  bitset<8> b1 = 0b11110000;

  b1.set();   // to 1 for all
  b1.reset(); // to 0 for all

  b1.set(1);    // 0000 0010
  b1[2] = 1;    // 0000 0110
  b1[0].flip(); // 0000 0111

  cout << b1 << endl;

  string s = b1.to_string();
  unsigned long n = b1.to_ulong();

  cout << s << endl;
  cout << n << endl;

  return 0;
}