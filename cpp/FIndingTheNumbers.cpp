// http://practice.geeksforgeeks.org/problems/finding-the-numbers/0

#include <cmath>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

int flip(int n, int bit) {
  int mask = 1 << bit;

  return n ^ mask;
}

void find(vector<int>& nums) {
  int table = 0;

  for (int i : nums) {
    table = flip(table, i);
  }

  int mask = 1;
  for (int i = 0; i < 10; ++i) {
    mask = 1;
    mask <<= i;

    if (mask & table) {
      cout << i << " ";
    }
  }
}

int main() {
  vector<int> nums = {2, 1, 3, 2};
  find(nums);

  // cout << flip(6, 0) << endl;
  return 0;
}