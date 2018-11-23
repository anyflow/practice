// https://practice.geeksforgeeks.org/problems/value-equal-to-index-value/0

#include "../common/practice.h"

using namespace std;

int main() {
  int T;
  auto results = vector<vector<int>>();

  cin >> T;

  while (T--) {
    auto result = vector<int>();

    int N, digit;

    cin >> N;

    for (int i = 1; i <= N; ++i) {
      cin >> digit;

      if (i == digit) {
        result.push_back(i);
      }
    }

    results.push_back(result);
  }

  for (auto item : results) {
    if (item.size() == 0) {
      cout << "Not Found" << endl;
    } else {
      for (auto num : item) {
        cout << num << " ";
      }

      cout << endl;
    }
  }

  return 0;
}
