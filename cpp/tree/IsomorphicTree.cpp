#include <climits>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

struct Node {
  int data;
  Node* left;
  Node* right;
};

bool isIsomorphic(Node* root1, Node* root2) {
  if (root1 == nullptr && root2 == nullptr) {
    return true;
  }
  if (root1 == nullptr || root2 == nullptr) {
    return false;
  }

  if (root1->data != root2->data) {
    return false;
  }

  return (isIsomorphic(root1->left, root2->left) &&
          isIsomorphic(root1->right, root2->right)) ||
         (isIsomorphic(root1->left, root2->right) &&
          isIsomorphic(root1->right, root2->left));
}

void insert(Node* root, int n1, int n2, char lr) {
  if (root == NULL)
    return;
  if (root->data == n1) {
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

Node* parse(string target) {
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
  //   auto tree1 = parse(string("1 2 L 1 3 R 2 4 L"));
  //   auto tree2 = parse(string("1 3 L 1 2 R 3 4 R"));

  //   auto tree1 = parse(string("1 2 L 1 3 R 2 4 L"));
  //   auto tree2 = parse(string("1 3 L 1 2 R 2 4 R"));

  auto tree1 = parse(string("0 1 L 0 2 R"));
  auto tree2 = parse(string("0 1 L 0 2 R"));

  string ret = isIsomorphic(tree1, tree2) ? "YES" : "NO";
  cout << ret << endl;

  return 0;
}