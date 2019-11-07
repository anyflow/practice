#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Invalid arguments count!" << endl;
    return -1;
  }

  string s;

  unordered_map<string, string> keywords;

  ifstream ifs(argv[1]);
  ofstream ofs(argv[2]);

  while (getline(ifs, s)) {
    if (s.empty()) {
      continue;
    }

    istringstream iss(s);
    string word;

    vector<string> words;

    while (iss >> word) {
      words.push_back(word);
    }

    if (words[0] == "define") {
      keywords[words[1]] = words[2];
    } else if (words[1] == "=") {
      words[2] = keywords[words[2]];

      for (auto& w : words) {
        ofs << w << " ";
      }

      ofs << endl;
    } else {
      cout << "Invalid words exist!" << endl;
      return -1;
    }
  }

  return 0;
}

// cmd : bin/cpp/project4 cpp/stl.lecture/hyunjeong.park_255416/input.txt
// cpp/stl.lecture/hyunjeong.park_255416/result.txt