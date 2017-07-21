// http://practice.geeksforgeeks.org/problems/counting-elements-in-two-arrays/1

#include "../common/practice.h"

using namespace std;

void countEleLessThanOrEqual(int arr1[], int arr2[], int m, int n) {
  auto A = map<int, int>();
  auto B = vector<int>(arr2, arr2 + n);

  sort(B.begin(), B.end());

  for (int i = 0; i < m; ++i) {
    A[arr1[i]] = 0;
  }

  auto it = A.begin();
  auto preIt = A.end();
  int bIndex = 0;

  while (it != A.end()) {
    int a = it->first;

    if (preIt != A.end()) {
      A[a] = A[preIt->first];
    }

    if (bIndex >= n) {
      preIt = it;
      ++it;
      continue;
    }

    for (; bIndex < n; ++bIndex) {
      if (B[bIndex] <= a) {
        A[a] = it->second + 1;
      } else {
        break;
      }
    }

    preIt = it;
    ++it;
  }

  for (int i = 0; i < m; ++i) {
    cout << A[arr1[i]] << " ";
  }
}

int main() {
  // int arr1[] = {1, 2, 3, 4, 7, 9};
  // int arr2[] = {0, 1, 2, 1, 1, 4};
  int arr1[] = {4, 8, 7, 5, 1};
  int arr2[] = {4, 48, 3, 0, 1, 1, 5};

  countEleLessThanOrEqual(arr1, arr2, sizeof(arr1) / sizeof(int),
                          sizeof(arr2) / sizeof(int));
  return 0;
}