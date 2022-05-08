// https://practice.geeksforgeeks.org/problems/how-many-xs/0

#include "../lib/practice.h"

using namespace std;

int findX(int num, int lower, int upper) {
  int ret = 0;

  for (auto i = lower + 1; i < upper; ++i) {
    for (auto c : to_string(i)) {
      if (num == c - 48) {
        ++ret;
      }
    }
  }

  return ret;
}

int main() {
  int T = 1;
  auto results = vector<int>();

  cin >> T;

  while (T--) {
    int num, lower, upper;

    cin >> num >> lower >> upper;

    results.push_back(findX(num, lower, upper));
  }

  for (auto item : results) {
    cout << item << endl;
  }

  return 0;
}