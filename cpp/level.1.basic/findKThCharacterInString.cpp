// https://practice.geeksforgeeks.org/problems/find-k-th-character-in-string/0

// 풀지 못함. editorial도 아래와 같은 brute-force를 논하는데, 항시 timeout으로
// pass 못함. editorial 대로 vector 대신 string으로도 해보았으나 실패.

#include "../lib/practice.h"

using namespace std;
using namespace anyflow;

vector<bool> iterate(vector<bool>& items) {
  auto ret = vector<bool>();

  for (bool item : items) {
    ret.push_back(item);
    ret.push_back(!item);
  }

  return ret;
}

vector<bool> iterateN(deque<bool>& items, int n) {
  auto ret = vector<bool>(items.begin(), items.end());

  while (n--) {
    ret = iterate(ret);
  }

  return ret;
}

vector<bool> updated(int m, int n) {
  auto bits = deque<bool>();

  int divided = m;

  while (true) {
    bits.push_front(divided % 2);

    divided = (int)(divided / 2);

    if (divided == 1) {
      bits.push_front(divided);
      break;
    }
  }

  return iterateN(bits, n);
}

int main() {
  int m = 11;
  int k = 6;
  int n = 4;

  auto stopwatch = Stopwatch();

  auto bits = updated(m, n);

  stopwatch.printElapsed();

  cout << bits[k] << endl;

  return 0;
}