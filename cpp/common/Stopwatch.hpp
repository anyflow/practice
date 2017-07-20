#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

struct Stopwatch {
  system_clock::time_point startTime;

  Stopwatch() : startTime(system_clock::now()) {}

  size_t elapsed() {
    return duration_cast<microseconds>(system_clock::now() - startTime).count();
  }

  void printElapsed() {
    cout << "elapsed time : " << elapsed() << "ms" << endl;
  }
};

#endif // STOPWATCH_HPP