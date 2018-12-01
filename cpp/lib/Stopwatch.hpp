#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>
#include <iostream>

class Stopwatch {
private:
  std::chrono::system_clock::time_point startTime;

public:
  Stopwatch() : startTime(std::chrono::system_clock::now()) {}

  size_t elapsed() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::system_clock::now() - startTime)
        .count();
  }

  void printElapsed() {
    std::cout << "elapsed time : " << elapsed() << "ms" << std::endl;
  }
};

#endif // STOPWATCH_HPP