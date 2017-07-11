#include <iostream>
#include <utility>
#include <vector>

using namespace std;

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
  auto input = vector<int>({3, 20, 8, 1, 4, 7, 9, 0, 8, 5});

  sort(input);

  for (auto& i : input) {
    cout << i << " ";
  }

  cout << endl;
  return 0;
}