#include "../../lib/practice.h"

using namespace std;
using namespace anyflow;

void sort(vector<int>& arr) {
  for (int targetPos = 1; targetPos < arr.size(); ++targetPos) {
    auto target = arr[targetPos];

    int j = targetPos - 1;
    while (j >= 0 && arr[j] > target) {
      arr[j + 1] = arr[j];
      --j;
    }
    arr[j + 1] = target;
  }
}

int main() {
  vector<int> input = {
      3,    1,    5,   9,    0,   10,  5,    32,  -1,  -20,  239, 583,
      6,    3,    54,  53,   38,  34,  653,  75,  345, 7576, 45,  27,
      4564, 8765, 24,  1,    435, 564, 2435, 654, 986, 345,  234, 3745,
      44,   33,   22,  77,   88,  99,  11,   22,  4,   5,    6,   8,
      3,    4,    6,   7443, 221, 4,   3475, 543, 769, 435,  145, 456,
      2463, 8789, 324, 1242, 34,  136, 678,  986, 44,  5463, 234, 789};

  auto stopwatch = Stopwatch();

  sort(input);

  stopwatch.printElapsed();

  for (auto& i : input) {
    cout << i << " ";
  }

  cout << endl;
  return 0;
}