// https://practice.geeksforgeeks.org/problems/how-many-xs/0

#include "../common/practice.h"

using namespace std;

int findX(int num, int lower, int upper) {
  cout << num << lower << upper << endl;

  return 0;
}

int main() {
  int T;
  auto results = vector<int>();

  cin >> T;

  while (T--) {
    int num, lower, upper;

    cin >> num >> lower >> upper;

    results.push_back(findX(num, lower, upper));
  }

  return 0;
}