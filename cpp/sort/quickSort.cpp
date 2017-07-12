#include <chrono>
#include <climits>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

void sort(vector<int>& arr, int startIndex, int endIndex) {
  if (endIndex - startIndex <= 1) {
    return;
  }

  int pivotIndex = (startIndex + endIndex) / 2;
  int pivot = arr[pivotIndex];

  vector<int> left, right, pivots;

  for (int i = startIndex; i < endIndex; ++i) {
    if (arr[i] == pivot) {
      pivots.push_back(arr[i]);
    } else if (arr[i] < pivot) {
      left.push_back(arr[i]);
    } else {
      right.push_back(arr[i]);
    }
  }

  pivotIndex =
      startIndex + left.size() + (pivots.size() > 0 ? pivots.size() - 1 : 0);

  left.insert(left.end(), pivots.begin(), pivots.end());
  left.insert(left.end(), right.begin(), right.end());

  copy(left.begin(), left.end(), arr.begin() + startIndex);

  sort(arr, startIndex, pivotIndex);
  sort(arr, pivotIndex, endIndex);
}

void sort(vector<int>& arr) { sort(arr, 0, arr.size()); }

int main() {
  vector<int> input = {
      3,    1,    5,   9,    0,   10,  5,    32,  -1,  -20,  239, 583,
      6,    3,    54,  53,   38,  34,  653,  75,  345, 7576, 45,  27,
      4564, 8765, 24,  1,    435, 564, 2435, 654, 986, 345,  234, 3745,
      44,   33,   22,  77,   88,  99,  11,   22,  4,   5,    6,   8,
      3,    4,    6,   7443, 221, 4,   3475, 543, 769, 435,  145, 456,
      2463, 8789, 324, 1242, 34,  136, 678,  986, 44,  5463, 234, 789};

  auto before = system_clock::now();
  sort(input);
  auto after = system_clock::now();

  auto elapsed = duration_cast<microseconds>(after - before).count();
  cout << "elapsed time : " << elapsed << endl;

  for (auto& i : input) {
    cout << i << " ";
  }

  cout << endl;
  return 0;
}