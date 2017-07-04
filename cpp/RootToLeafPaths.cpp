// question :
// http://practice.geeksforgeeks.org/problems/root-to-leaf-paths/1

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
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

using Path = vector<Node*>;

void printPaths(Node* root) {
  auto paths = map<Node*, Path*>();
  auto parents = map<Node*, Node*>();
  auto leafPaths = vector<Path*>();

  auto q = queue<Node*>();
  q.push(root);

  while (!q.empty()) {
    auto node = q.front();

    auto path = parents.find(node) != parents.end()
                    ? new Path(paths[parents[node]]->begin(),
                               paths[parents[node]]->end())
                    : new Path();
    path->push_back(node);

    paths[node] = path;

    if (!node->left && !node->right) {
      leafPaths.push_back(path);
    }

    if (node->left) {
      parents[node->left] = node;
      q.push(node->left);
    }
    if (node->right) {
      parents[node->right] = node;
      q.push(node->right);
    }

    q.pop();
  }

  for (auto path : leafPaths) {
    for (auto i : *path) {
      cout << i->data << " ";
    }
    cout << "# ";
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
      vector<string>{"1 2 R 1 3 L", "10 20 L 10 30 R 20 40 L 20 60 R"};

  for (auto item : testcases) {
    printPaths(parse(item));
    cout << endl;
  }

  return 0;
}