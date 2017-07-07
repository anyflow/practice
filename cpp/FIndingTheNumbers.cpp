// http://practice.geeksforgeeks.org/problems/finding-the-numbers/0

#include <cmath>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

using namespace std;

void find(vector<int>& nums) {
  auto vals = unordered_set<int>();

  for (int i : nums) {
    if (vals.find(i) != vals.end()) {
      vals.erase(i);
    } else {
      vals.insert(i);
    }
  }

  auto ret = set<int>(vals.begin(), vals.end());

  for (int i : ret) {
    cout << to_string(i) << " ";
  }
}

int main() {
  vector<int> nums = {1, 2, 3, 2, 1, 4};
  find(nums);

  cout << endl;

  return 0;
}