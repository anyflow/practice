// question :
// http://practice.geeksforgeeks.org/problems/root-to-leaf-paths/1

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
struct Node {
  int data;
  Node* left;
  Node* right;
};

struct NodeEx : Node {
  NodeEx* parent;

  NodeEx() {}
  NodeEx(Node* node, NodeEx* parent)
      : Node{node->data, node->left, node->right}, parent(parent) {}
};

struct NodeComp {
  bool operator()(const NodeEx* lhs, const NodeEx* rhs) const {
    auto l = (NodeEx*)lhs;
    auto r = (NodeEx*)rhs;
    auto preL = (NodeEx*)lhs;
    auto preR = (NodeEx*)rhs;

    l = l->parent;

    if (l == r) {
      return preL->data == l->left->data;
    }

    while (l != nullptr) {
      while (r != nullptr) {
        if (l == r) {
          return preL->data == l->left->data;
        }

        preR = r;
        r = r->parent;
      }
      preL = l;
      l = l->parent;

      r = preR = (NodeEx*)rhs;
    }

    return false;
  }
};

void printPaths(Node* root) {
  auto leafs = set<NodeEx*, NodeComp>();

  auto q = queue<NodeEx*>();
  q.push(new NodeEx(root, nullptr));

  while (!q.empty()) {
    auto node = q.front();

    if (!node->left && !node->right) {
      leafs.insert(node);
    }

    if (node->left) {
      q.push(new NodeEx(node->left, node));
    }
    if (node->right) {
      q.push(new NodeEx(node->right, node));
    }

    q.pop();
  }

  string ret;
  for (auto item : leafs) {
    auto s = stack<NodeEx*>();

    auto node = item;
    s.push(node);

    while (node->parent != nullptr) {
      node = node->parent;

      s.push(node);
    }

    while (!s.empty()) {
      cout << s.top()->data << " ";
      ret += to_string(s.top()->data) + " ";
      s.pop();
    }

    cout << "#";
    ret += "#";
  }
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
  auto testcases =
      vector<string>{"1 2 R 1 3 L", "10 20 L 10 30 R 20 40 L 20 60 R",
                     "1 2 L 1 3 R 2 4 L 2 5 R 3 6 L 3 7 R"};

  for (auto item : testcases) {
    printPaths(parse(item));
    cout << endl;
  }

  return 0;
}