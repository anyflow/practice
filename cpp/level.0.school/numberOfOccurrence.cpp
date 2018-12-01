// https://practice.geeksforgeeks.org/problems/replace-all-0-with-5-in-an-input-integer/1

#include "../lib/practice.h"

using namespace std;

int main() {
  int T;
  vector<int> results;

  cin >> T;

  while (T--) {
    int N, num, digit, count = 0;

    cin >> N >> num;

    for (int i = 0; i < N; ++i) {
      cin >> digit;

      if (digit == num) {
        ++count;
      }
    }

    if (count == 0) {
      count = -1;
    }

    results.push_back(count);
  }

  for (auto item : results) {
    cout << item << endl;
  }

  return 0;
}
