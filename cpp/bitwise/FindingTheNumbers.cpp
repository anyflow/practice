// http://practice.geeksforgeeks.org/problems/finding-the-numbers/0

#include "../common/practice.h"

using namespace std;

int find(vector<int>& nums) {
  int mask = 0;
  for (auto i : nums) {
    mask ^= i;
  }

  return mask;
}

int main() {
  vector<int> nums = {468, 335, 501, 170, 725, 479, 359, 963, 465, 706, 146,
                      282, 828, 962, 492, 996, 943, 828, 437, 392, 605, 943,
                      468, 335, 501, 170, 725, 479, 359, 963, 465, 706, 146,
                      282, 828, 962, 492, 996, 943, 828, 437, 392, 605};

  cout << to_string(find(nums)) << endl;

  return 0;
}