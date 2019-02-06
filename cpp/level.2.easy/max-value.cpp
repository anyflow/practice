// https://practice.geeksforgeeks.org/problems/max-value/0/?ref=self
// Brute Force(O(n2))로 풀었으나, 아래는 정답임(O(n))

#include "../lib/practice.h"
#include <climits>
using namespace std;
using namespace anyflow;

int max_of(vector<int> arr);

// int main(int argc, char* argv[]) {
//   int T, N, temp;

//   cin >> T;

//   vector<int> arr;

//   while (T--) {
//     cin >> N;

//     arr.clear();
//     while (N--) {
//       cin >> temp;

//       arr.push_back(temp);
//     }

//     cout << max_of(arr) << endl;
//   }

//   return 0;
// }

int max_of(vector<int> arr) {
  int minVal = INT_MAX, maxVal = INT_MIN;

  for (int i = 0; i < arr.size(); ++i) {
    minVal = min(minVal, arr[i] - i);
    maxVal = max(maxVal, arr[i] - i);
  }

  return maxVal - minVal;
}

int main() {
  auto t1 = "9 15 4 12 13";
  auto t2 =
      "760 394 549 274 793 194 934 138 104 691 212 695 669 628 499 990 249 880 "
      "648 150 932 670 876 599 450 437 600 124 444 732 155 862 435 386 968";
  auto t3 = "706 627 813 376 94";

  auto arr = string2vector<int>(t3);

  cout << max_of(arr) << endl;
}