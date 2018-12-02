// https://practice.geeksforgeeks.org/problems/find-k-th-character-in-string/0

// TODO correct answer but time out occurred!

#include "../lib/practice.h"

using namespace std;

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

  auto bits = updated(m, n);

  cout << bits[k] << endl;

  return 0;
}

// int main() {
//   int T;
//   cin >> T;

//   auto rets = vector<bool>();

//   while (T--) {
//     int m, k, n;
//     cin >> m >> k >> n;

//     auto bits = updated(m, n);

//     rets.push_back(bits[k]);
//   }

//   for (auto ret : rets) {
//     cout << ret << endl;
//   }
// }