// https://practice.geeksforgeeks.org/problems/flip-bits/0

#include "../lib/practice.h"

using namespace std;

int flipCount(vector<int>& arr) {
  int count1 = 0;

  return 0;
}

vector<int> string2vector(string input) {
  vector<int> ret;

  stringstream stream(input);
  string temp;

  while (getline(stream, temp, ' ')) {
    ret.push_back(std::stoi(temp));
  }

  return ret;
}

int main(int argc, char* argv[]) {
  // int T;

  // cin >> T;

  // while (T--) {
  //   int count;

  //   cin >> count;

  //   vector<int> arr;

  //   while (count--) {
  //     int item;
  //     cin >> item;

  //     arr.push_back(item);
  //   }

  //   cout << flipCount(arr) << endl;
  // }

  // auto arr = vector<int>{0, 1, 1, 1, 1, 0, 0, 0};

  auto arr = string2vector("");

  cout << flipCount(arr) << endl;

  return 0;
}
