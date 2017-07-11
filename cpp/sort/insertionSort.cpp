#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

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
  vector<int> input = {3,   1,   5,   9, 0, 10, 5,  32, -1,
                       -20, 239, 583, 6, 3, 54, 53, 38};

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