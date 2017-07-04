// question :
// http://practice.geeksforgeeks.org/problems/leftmost-and-rightmost-nodes-of-binary-tree/1

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

int sum(Path* path) {
  int ret = 0;
  for (auto item : *path) {
    ret += item->data;
  }

  return ret;
}

void bfs(int k, Node* startNode, vector<Path*>& sums) {
  auto paths = map<Node*, Path*>();
  auto visited = map<Node*, bool>();
  auto parents = map<Node*, Node*>();

  auto q = queue<Node*>();
  q.push(startNode);

  while (!q.empty()) {
    auto node = q.front();

    if (visited[node]) {
      q.pop();
      continue;
    }
    visited[node] = true;

    auto path = parents.find(node) != parents.end()
                    ? new Path(paths[parents[node]]->begin(),
                               paths[parents[node]]->end())
                    : new Path();
    path->push_back(node);

    paths[node] = path;

    if (sum(path) == k) {
      sums.push_back(path);
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
}

int printCount(Node* root, int k) {
  auto sums = vector<Path*>();

  auto q = queue<Node*>();
  q.push(root);

  while (!q.empty()) {
    root = q.front();
    bfs(k, root, sums);

    if (root->left) {
      q.push(root->left);
    }
    if (root->right) {
      q.push(root->right);
    }

    q.pop();
  }

  for (auto item : sums) {
    cout << "size : " << item->size() << " | ";

    for (auto node : *item) {
      cout << node->data << " ";
    }
    cout << endl;
  }
  return sums.size();
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
  auto testcases = map<int, string>();
  testcases[4] = "1 3 L 3 2 L 3 -1 R -1 1 R";
  testcases[3] = "0 1 L 0 2 R 1 3 L 1 4 R 2 5 L 2 6 R";

  for (auto item : testcases) {
    cout << "k=" << item.first << "|tree:" << item.second << endl;
    cout << printCount(parse(item.second), item.first) << endl;
  }

  return 0;
}