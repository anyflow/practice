#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <algorithm>
#include <iostream>

template <typename Container> void show(Container container) {
  for_each(container.begin(), container.end(),
           [](int n) { std::cout << n << std::endl; });
};

#endif