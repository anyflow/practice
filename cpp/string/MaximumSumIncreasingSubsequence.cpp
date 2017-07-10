#include <iostream>
#include <set>
#include <vector>

// question :
using namespace std;

int get(vector<int> arr) {}

int main() {
  auto arrs = vector<vector<int>>{{1, 101, 2, 3, 100, 4, 5}, {10, 5, 4, 3}};

  for (auto arr : arrs) {
    for (auto i : arr) {
      cout << i << " ";
    }

    cout << endl;
  }
  return 0;
}