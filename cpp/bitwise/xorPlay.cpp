// http://practice.geeksforgeeks.org/problems/xor-play/0

#include "../common/practice.h"

using namespace std;

void get(int n) {
  if (n == 1) {
    cout << 1 << endl;
    cout << 1 << endl;
    return;
  }

  auto divisors = set<int>();
  divisors.insert(1);

  auto sq = (int)sqrt(n);
  int opXor = 1;

  for (int i = 2; i <= sq; ++i) {
    if (n % i == 0) {
      divisors.insert(i);
      divisors.insert(n / i);

      opXor ^= i;
      if (i != n / i) {
        opXor ^= (n / i);
      }
    }
  }

  for (auto i : divisors) {
    cout << to_string(i) << " ";
  }

  cout << endl;
  cout << to_string(opXor) << endl;
}

int main() {
  get(100);
  return 0;
}