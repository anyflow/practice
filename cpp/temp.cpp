#include <algorithm>
#include <iostream>

using namespace std;

int main() {
  vector<int> array = {1, 5, 3, 10, 38, 27, 68, 7, 38, 5};

  sort(array.begin(), array.end() - 3, [](auto a, auto b) { return b > a; });

  for (auto i : array) {
    cout << i << endl;
  }
}