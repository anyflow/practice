#include <bitset>
#include <iostream>
#include <string>
#include <typeinfo>

using namespace std;

struct Temp {
  int val = 1;
};

int main() {
  for (auto& i : {1}) {
    cout << to_string(i) << " | type : " << typeid(i).name() << endl;
  }

  int intVal = 2;

  cout << typeid(Temp).name() << endl;
  return 0;
}