// https://practice.geeksforgeeks.org/problems/floor-in-a-sorted-array/0/?ref=self
// result : OK

#include "../lib/practice.h"
using namespace std;
using namespace anyflow;

int floorOf(vector<u_int64_t> arr, u_int64_t x, u_int64_t baseIndex);

int main() {
  auto t2 = string2vector<u_int64_t>(
      "66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 "
      "90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 "
      "110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 "
      "128 129 130");

  cout << floorOf(t2, 106, 0) << endl;
  return 0;
}

int floorOf(vector<u_int64_t> arr, u_int64_t x, u_int64_t baseIndex) {
  if (arr.size() <= 0 || arr[0] > x) {
    return -1;
  }

  if (arr[arr.size() - 1] < x) {
    return baseIndex + arr.size() - 1;
  }

  u_int64_t centerIndex = arr.size() / 2;
  u_int64_t centerVal = arr[centerIndex];

  if (arr.size() == 1 || x == centerVal) {
    return baseIndex + centerIndex;
  } else if (x < centerVal) {
    return floorOf(vector<u_int64_t>(arr.begin(), arr.begin() + centerIndex), x,
                   baseIndex);
  } else {
    return floorOf(
        vector<u_int64_t>(arr.begin() + centerIndex, arr.begin() + arr.size()),
        x, baseIndex + centerIndex);
  }
}