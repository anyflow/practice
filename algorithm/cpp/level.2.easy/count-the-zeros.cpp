// https://practice.geeksforgeeks.org/problems/count-the-zeros/0
// Ressult : OK

#include "../lib/practice.h"

using namespace std;
using namespace anyflow;

int process(vector<int> arr, int baseIndex);

int main() {
  int T;

  cin >> T;

  vector<int> arr;
  while (T--) {
    int n, temp;

    cin >> n;

    arr.clear();
    while (n--) {
      cin >> temp;

      arr.push_back(temp);
    }

    auto baseIndex = process(arr, 0);

    int ret = 0;

    if (baseIndex == -1) {
      ret = 0;
    } else {
      ret = arr.size() - baseIndex;
    }

    cout << ret << endl;
  }

  return 0;
}

int process(vector<int> arr, int baseIndex) {
  if (arr.size() <= 0 || arr[arr.size() - 1] == 1) {
    return -1;
  }

  if (arr[0] == 0) {
    return baseIndex;
  }

  if (arr.size() == 2 && arr[0] == 1 && arr[1] == 1) {
    return baseIndex + 1;
  }

  int centerIndex = arr.size() / 2;
  int centerVal = arr[centerIndex];

  if (centerVal == 0) {
    int idx =
        process(vector<int>(arr.begin(), arr.begin() + centerIndex), baseIndex);

    return idx == -1 ? baseIndex + centerIndex : idx;
  } else {
    return process(vector<int>(arr.begin() + centerIndex, arr.end()),
                   baseIndex + centerIndex);
  }
}
