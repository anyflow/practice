// https://practice.geeksforgeeks.org/problems/element-appearing-once/0/?ref=self
// result : FAILED

#include "../lib/practice.h"
using namespace std;
using namespace anyflow;

int onceValue(vector<int> arr);

int main() {
  auto t2 = string2vector<int>("2 2 5 5 20 30 30");

  cout << onceValue(t2) << endl;
  return 0;
}

int onceValue(vector<int> arr) {
  int t = 0;

  for (int i : arr) {
    t ^= i;
  }

  return t;
}