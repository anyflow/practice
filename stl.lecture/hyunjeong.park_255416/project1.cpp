#include <iostream>

using namespace std;

template <typename InputItr, typename OutputItr, typename Predicate>
OutputItr xcopy_if(InputItr begin, InputItr end, OutputItr result,
                   Predicate predicate) {
  for (; begin != end; ++begin) {
    if (!predicate(*begin)) {
      continue;
    }

    *result = *begin;
    ++result;
  }

  return result;
}

int main() {
  int x[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int y[10] = {0};

  xcopy_if(x, x + 10, y, [](int a) { return a % 2 == 0; });

  for (auto n : y)
    cout << n << endl;
  // 출력 결과 : 2, 4, 6, 8, 10, 0,0,0,0,0
}