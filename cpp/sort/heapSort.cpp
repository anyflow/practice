#include <algorithm>
#include <chrono>
#include <climits>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

struct Heap {
  vector<int> items;

  void push(int n) {}

  int pop() {
    if (items.size() == 0) {
      return INT_MIN;
    }

    int ret = items[0];

    swap(ret[0], ret[items.end() - 1]);
    items.pop_back();

    return ret;
  }
};

void sort(vector<int>& arr) {
  Heap heap;

  for (auto& i : arr) {
    heap.push(i);
  }

  for (int i = 0; i < arr.size(); ++i) {
    arr[i] = heap.pop();
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