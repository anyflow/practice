#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace anyflow {

template <class _T> std::vector<_T> string2vector(std::string input) {
  std::vector<_T> ret;

  std::stringstream stream(input);
  std::string temp;

  while (getline(stream, temp, ' ')) {
    ret.push_back(std::stoi(temp));
  }

  return ret;
}

} // namespace anyflow

#endif // #ifndef UTIL_HPP