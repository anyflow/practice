#include <iostream>
#include <limits>
#include <vector>

using namespace std;

int main() {
  enum class Method { gauss_seidel, jacobi };

  double solution_x1 = 1, solution_x2 = 2;

  double x1 = 0, x2 = 0;
  double temp_x1 = x1, temp_x2 = x2;

  Method method = Method::jacobi;

  int count = 1;

  cout.precision(numeric_limits<double>::max_digits10);

  while (x1 != solution_x1 || x2 != solution_x2) {
    switch (method) {
    case Method::gauss_seidel:
      temp_x1 = (4 - x2) / 2;
      temp_x2 = (5 - x1) / 2;

      x1 = temp_x1;
      x2 = temp_x2;
      break;

    case Method::jacobi:
      x1 = (4 - x2) / 2;
      x2 = (5 - x1) / 2;
      break;

    default:
      throw "unknown method!!";
    }

    cout << "count : " << count++ << endl;
    cout << "x1 : " << fixed << x1 << endl;
    cout << "x2 : " << fixed << x2 << endl;
  }

  return 0;
}