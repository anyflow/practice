#include <algorithm>
#include <chrono>
#include <climits>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

void sort(vector<int>& arr, int startIndex, int endIndex) {
  if (endIndex <= startIndex + 1) {
    return;
  }

  int middleIndex = (startIndex + endIndex - 1) / 2;

  sort(arr, startIndex, middleIndex);
  sort(arr, middleIndex, endIndex);

  auto merge = [&]() {
    vector<int> ret;

    int i = startIndex, j = middleIndex;

    while (i < middleIndex || j < endIndex) {
      if (i == middleIndex) {
        ret.insert(ret.end(), arr.begin() + middleIndex, arr.end() + endIndex);
        break;
      }
      if (j == endIndex) {
        ret.insert(ret.end(), arr.begin() + middleIndex, arr.end() + endIndex);
        break;
      }

      if (arr[i] > arr[j]) {
        ret.push_back(arr[j]);
        ++j;
      } else if (arr[i] == arr[j]) {
        ret.insert(ret.end(), {arr[i], arr[j]});
        ++i;
        ++j;
      } else {
        ret.push_back(arr[i]);
        ++i;
      }
    }

    copy(ret.begin(), ret.end(), arr.begin() + startIndex);
  };

  merge();
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