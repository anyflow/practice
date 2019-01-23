#include "../lib/practice.h"

using namespace std;

// A Kadane's algorithm based solution to find maximum
// number of 0s by flipping a subarray.
int findMaxZeroCount(bool arr[], int n) {
  // Initialize count of zeros and maximum difference
  // between count of 1s and 0s in a subarray
  int orig_zero_count = 0;

  // Initiale overall max diff for any subarray
  int max_diff = 0;

  // Initialize current diff
  int curr_max = 0;

  for (int i = 0; i < n; i++) {
    // Count of zeros in original array (Not related
    // to Kadane's algorithm)
    if (arr[i] == 0)
      orig_zero_count++;

    // Value to be considered for finding maximum sum
    int val = (arr[i] == 1) ? 1 : -1;

    // Update current max and max_diff
    curr_max = max(val, curr_max + val);
    max_diff = max(max_diff, curr_max);

    i = i;
  }
  max_diff = max(0, max_diff);

  return orig_zero_count + max_diff;
}

// Driver program
int main() {

  // bool arr[] = {0, 1, 0, 0, 1, 1, 0};
  // int n = sizeof(arr) / sizeof(arr[0]);
  // cout << findMaxZeroCount(arr, n);

  return 0;
}