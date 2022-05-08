// https://practice.geeksforgeeks.org/problems/flip-bits/0
// FAILED

#include "../lib/practice.h"

using namespace std;
using namespace anyflow;

int flipCount(vector<int>& arr) {
  int val;
  int count1 = 0;
  int currentMax = 0;
  int maxDiff = 0;

  for (int i = 0; i < arr.size(); ++i) {
    val = arr[i];
    if (val == 1) {
      ++count1;
    }

    val = val == 1 ? -1 : 1;

    currentMax = std::max(val, currentMax + val);
    maxDiff = std::max(maxDiff, currentMax);
  }

  return count1 + std::max(0, maxDiff);
}

int main(int argc, char* argv[]) {
  // int T;

  // cin >> T;

  // while (T--) {
  //   int count;

  //   cin >> count;

  //   vector<int> arr;

  //   while (count--) {
  //     int item;
  //     cin >> item;

  //     arr.push_back(item);
  //   }

  //   cout << flipCount(arr) << endl;
  // }

  auto arr = anyflow::string2vector<int>("0 1 1 1 1 0 0 0");

  cout << flipCount(arr) << endl;

  return 0;
}
