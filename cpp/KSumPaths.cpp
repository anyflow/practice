// question :
// http://practice.geeksforgeeks.org/problems/leftmost-and-rightmost-nodes-of-binary-tree/1

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

struct GNode {
  int data;
  GNode* parent;
  vector<GNode*> list;
};

struct Node {
  int key;
  Node* left;
  Node* right;
};

auto toGraph(Node* root) {
  auto ret = map<Node*, GNode*>();

  auto q = queue<Node*>();

  q.push(root);
  ret[root] = new GNode{root->key};

  while (!q.empty()) {
    root = q.front();

    if (root->left) {
      ret[root->left] = new GNode{root->left->key};
      ret[root->left]->list.push_back(ret[root]);

      ret[root]->list.push_back(ret[root->left]);

      q.push(root->left);
    }

    if (root->right) {
      ret[root->right] = new GNode{root->right->key};
      ret[root->right]->list.push_back(ret[root]);

      ret[root]->list.push_back(ret[root->right]);

      q.push(root->right);
    }

    q.pop();
  }

  return ret;
}

bool isSame(vector<GNode*>* a, vector<GNode*>* b) {
  if (a->size() != b->size()) {
    return false;
  }

  auto comp = [](auto l, auto r) { return l->data - r->data; };

  sort(a->begin(), a->end(), comp);
  sort(b->begin(), b->end(), comp);

  for (int i = 0; i < a->size(); ++i) {
    if ((*a)[i]->data != (*b)[i]->data) {
      return false;
    }
  }

  return true;
}

int sum(auto path) {
  int ret = 0;
  for (auto item : *path) {
    ret += item->data;
  }

  return ret;
}

auto bfs(int k, GNode* startNode) {
  auto paths = map<GNode*, vector<GNode*>*>();
  auto sums = vector<vector<GNode*>*>();
  auto visited = map<GNode*, bool>();

  auto q = queue<GNode*>();
  q.push(startNode);

  while (!q.empty()) {
    auto node = q.front();

    if (visited[node]) {
      q.pop();
      continue;
    }
    visited[node] = true;

    auto path = new vector<GNode*>();
    path->push_back(node);

    if (node->parent != nullptr) {
      copy(paths[node->parent]->begin(), paths[node->parent]->end(),
           path->begin());
    }

    paths[node] = path;

    if (sum(path) == k) {
      sums.push_back(path);
    }

    for (auto item : node->list) {
      q.push(item);
    }

    q.pop();
  }

  return sums;
}

int printCount(Node* root, int k) {
  auto g = toGraph(root);

  auto node = g.begin()->second;

  return -1;
}

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