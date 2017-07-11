#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

void sort(vector<int>& arr) {
  for (int i = 0; i < arr.size(); ++i) {
    for (int j = 1; j < arr.size() - i; ++j) {
      if (arr[j - 1] > arr[j]) {
        swap(arr[j - 1], arr[j]);
      }
    }
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