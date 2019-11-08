#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>

using namespace std;

struct abs_greater {
  bool operator()(int a, int b) const { return abs(a) > abs(b); }
};

map<string, priority_queue<int, vector<int>>> map1;

int main() {

  map1["1"].push(10); // <      >
  map1["1"].push(20);
  map1["1"].push(-15);
  map1["1"].push(12);

  auto pq = map1["1"];

  cout << pq.top() << endl; // 20    10
  pq.pop();
  cout << pq.top() << endl; // 15    12

  cout << "pq size : " << pq.size() << endl;
  cout << "map1[\"1\"] size : " << map1["1"].size() << endl;

  cout << "address map1[\"1\"] : " << addressof(map1["1"]) << endl;
  cout << "address pq : " << addressof(pq) << endl;
}
