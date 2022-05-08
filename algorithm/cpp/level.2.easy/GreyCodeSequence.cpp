// http://practice.geeksforgeeks.org/problems/generate-grey-code-sequences/1

#include "../lib/practice.h"

using namespace std;

void get(vector<int>& prev, int pos) {
  int prefix = 1 << (pos - 1);

  vector<int> news;
  for (int i = prev.size() - 1; i >= 0; --i) {
    news.push_back(prefix + prev[i]);
  }

  prev.insert(prev.end(), news.begin(), news.end());
}

string bin(int N, int num) {
  auto ret = string(N, '0');

  int mask = 1 << N;

  int pos = 0;
  while (mask >>= 1) {
    ret[pos++] = num & mask ? '1' : '0';
  }

  return ret;
}

int main() {

  auto list = vector<int>();

  list.push_back(0b0);
  list.push_back(0b1);

  for (int i = 2; i <= 4; ++i) {
    get(list, i);
  }

  for (auto i : list) {
    cout << bin(4, i) << " ";
  }

  return 0;
}