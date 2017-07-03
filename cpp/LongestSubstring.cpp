// question :
// http://practice.geeksforgeeks.org/problems/longest-distinct-characters-in-string/0

#include <iostream>
#include <map>
#include <string>

using namespace std;

string windowString(map<char, int>& window) {
  string ret = string(window.size(), '-');

  for (auto it = window.begin(); it != window.end(); ++it) {
    ret[it->second] = it->first;
  }

  return ret;
}

pair<int, string> maxDistinctWindow(string target) {
  map<int, string> windows;
  map<char, int> window;

  int startIndex = 0;
  while (startIndex < target.length()) {
    window.clear();

    for (int i = startIndex; i < target.length(); ++i) {
      char ch = target[i];

      if (window.find(ch) != window.end()) {
        startIndex = window[ch] + startIndex + 1;
        string str = windowString(window);
        windows[str.length()] = str;
        break;
      } else {
        window[ch] = i - startIndex;

        if (i == target.length() - 1) {
          startIndex = target.length();
          string str = windowString(window);
          windows[str.length()] = str;
          break;
        }
      }
    }
  }

  return windows.size() <= 0 ? pair<int, string>(0, "")
                             : pair<int, string>(windows.rbegin()->first,
                                                 windows.rbegin()->second);
}

int main() {
  string testcases[] = {"", "a", "abababcdefababcdab", "geeksforgeeks"};
  // 0, 1, 6, 7

  for (auto item : testcases) {
    auto answer = maxDistinctWindow(item);

    cout << "window : " << answer.second << " / size : " << answer.first
         << endl;

    return 0;
  }
}