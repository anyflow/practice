#include "../lib/practice.h"

using namespace std;

int main() {
  int T, size, temp, max, current;
  cin >> T;

  while (T > 0) {
    cin >> size;

    if (size <= 0) {
      cout << -1 << endl;
      continue;
    }

    max = current = 0;

    int startMax = 0;
    bool isStart = false;
    bool isEnd = false;

    cin >> temp;
    if (temp == 0) {
      isStart = true;
      max = current = 1;
      startMax = 1;
    }

    for (int i = 1; i < size; ++i) {
      cin >> temp;

      if (temp == 0) {
        ++current;
        isEnd = i == size - 1;
      } else {
        max = std::max(startMax, std::max(max, current));

        if (isStart) {
          startMax = max;
        }
        isStart = false;

        current = 0;
        isEnd = false;
      }
    }

    max = std::max(max, current);

    if (startMax == max) {
      isStart = true;
    }

    if (isStart && isEnd) {
      cout << -1 << endl;
    } else if (isStart || isEnd) {
      cout << max << endl;
    } else {
      cout << (int)ceil((double)max / (double)2) << endl;
    }

    --T;
  }
}