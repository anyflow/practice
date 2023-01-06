// https://leetcode.com/explore/learn/card/array-and-string/204/conclusion/1182/

#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  void rotate(vector<int>& nums, int k) {
    if (k == 0) {
      return;
    }

    // solution #1
    //  time complexity : O(N)
    //  space complexity : O(1)
    //  for(int i=0; i<k; ++i) {
    //      nums.insert(nums.begin(), nums.back());
    //      nums.pop_back();
    //  }

    // solution #2
    //  time complexity : O(1)
    //  space complexity : O(N)
    k = k % nums.size();

    // nums.insert(nums.begin(), nums.end() - k, nums.end());
    // nums.erase(nums.end() - k, nums.end());
    auto nums_new = vector<int>(nums.end() - k, nums.end());
    nums_new.insert(nums.end(), nums.begin(), nums.end() - k);

    for (auto& i : nums_new) {
      cout << i << "|";
    }
  }
};

void print(vector<int>& items) {
  cout << "[";
  for (auto i = 0; i < items.size(); ++i) {
    cout << items[i];
    if (i != items.size() - 1) {
      cout << ", ";
    }
  }

  cout << "]" << endl;
}

int main() {
  vector<int> nums = {1, 2, 3};

  nums.erase

          cout
      << "input: " << endl;
  print(nums);

  Solution().rotate(nums, 1);

  cout << "output: " << endl;
  print(nums);

  return 0;
}