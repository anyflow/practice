// http://practice.geeksforgeeks.org/problems/longest-palindromic-subsequence/0

#include "../lib/practice.h"

using namespace std;

string reverse(string in) {
  string ret;
  for (auto it = in.rbegin(); it != in.rend(); ++it) {
    ret += *it;
  }

  return ret;
}

pair<int, string> maxPalindromicSubstring(string& target) {
  auto ret = map<int, string>();
  string lastHalf;

  for (auto i = 0; i < target.length(); ++i) {
    lastHalf.clear();

    for (auto j = 0; j < i + 1; ++j) {
      lastHalf.insert(0, 1, target[j]);
    }

    for (int t = 0; t < 2; ++t) {
      if (i + 1 + t >= target.length()) {
        break;
      }

      string firstHalf = target.substr(i + 1 + t);
      int size = min(firstHalf.size(), lastHalf.size());

      string first = firstHalf.substr(0, size);
      string last = lastHalf.substr(0, size);

      int sameSize = 0;
      for (int c = 0; c < size; ++c) {
        if (first[c] != last[c]) {
          break;
        }
        ++sameSize;
      }

      int stringSize = t == 0 ? sameSize * 2 : sameSize * 2 + 1;
      string val = t == 0 ? reverse(first.substr(0, sameSize)) +
                                first.substr(0, sameSize)
                          : reverse(first.substr(0, sameSize)) + target[i + 1] +
                                first.substr(0, sameSize);
      ret[stringSize] = val;
    }
  }

  return ret.size() > 0
             ? pair<int, string>(ret.rbegin()->first, ret.rbegin()->second)
             : pair<int, string>(0, "");
}

int main() {
  string testcases[] = {
      "",
      "a",
      "ab",
      "aba",
      "abba",
      "bbabcbcab",
      "abbaab",
      "tctiocgualxsmoqahpbcdvvugpjovhvomqzasfwtswnekehttkxygtsoiddfmauyqvaicxdv"
      "trbfvizqtwozpipansfzsbakyatbxywspxymixcbvqcnarpnmwpgapryrmzrkxjcxjofirie"
      "jmtkfiztgoagftfzhgqtgbvdnkkvdubmiuyneyjlojrueyvlfnenocsbmczryadgvevzceks"
      "pcotcjghwlwnnoqbspsqqxynbuofazyqdmlfwspufnhucyvuppnhmluqhlviktypgjwebnyi"
      "bicfixbzoohbadrhooraiprqbqwedwngepnoppoddvgfyzppogpyyjozzkdeiqlniydxnrat"
      "mjanipeyyvwwemvdwyifrvuzvxwlryedjesuwysuupqydnecnohejdfecdptcvzncthysauo"
      "rmmuzswmgftpkawneniijhvlddjvddjwrxsqspeywzqha"};

  for (auto test : testcases) {
    auto val = maxPalindromicSubstring(test);
    cout << "val : " << val.second << " / size : " << val.first << endl;
  }
}