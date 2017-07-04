// question :
// http://practice.geeksforgeeks.org/problems/leftmost-and-rightmost-nodes-of-binary-tree/1

#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

struct Node {
  int key;
  Node* left;
  Node* right;
};

int printCount(Node* root, int k) { return -1; }

void insert(Node* root, int n1, int n2, char lr) {
  if (root == NULL)
    return;
  if (root->key == n1) {
    switch (lr) {
    case 'L':
      root->left = new Node{n2, nullptr, nullptr};
      break;
    case 'R':
      root->right = new Node{n2, nullptr, nullptr};
      break;
    }
  } else {
    insert(root->left, n1, n2, lr);
    insert(root->right, n1, n2, lr);
  }
}

Node* parse(string& target) {
  stringstream stream(target);
  string temp;
  vector<string> list;

  while (getline(stream, temp, ' ')) {
    list.push_back(temp);
  }

  int n1 = -1, n2 = -1;

  Node* root = nullptr;

  int count = 0;
  for (auto item : list) {
    ++count;

    switch (count % 3) {
    case 0:
      if (root == nullptr) {
        root = new Node{n1, nullptr, nullptr};
      }

      insert(root, n1, n2, item[0]);
      break;

    case 1:
      n1 = stoi(item);
      break;

    case 2:
      n2 = stoi(item);
      break;
    }
  }

  return root;
}

int main(int argc, char* argv[]) {
  string testcases[] = {"15 10 L 10 8 L 10 12 R 15 20 R 20 16 L 20 25 R",
                        "4 1 L 4 2 R 1 6 L 1 10 R 2 2 L 2 3 R 6 9 L 6 1 R 10 "
                        "10 L 10 3 R 2 5 L 2 10 R 3 1 L 3 10 R"};

  int k = 4;

  for (auto item : testcases) {
    cout << printCount(parse(item), k) << endl;
  }

  return 0;
}