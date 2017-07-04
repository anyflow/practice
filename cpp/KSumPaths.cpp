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

struct Vertex {
  int data;
  Vertex* parent;
  vector<Vertex*> list;
};

struct Node {
  int key;
  Node* left;
  Node* right;
};

map<Node*, Vertex*> toGraph(Node* root) {
  auto ret = map<Node*, Vertex*>();

  auto q = queue<Node*>();

  q.push(root);
  ret[root] = new Vertex{root->key};

  while (!q.empty()) {
    root = q.front();

    if (root->left) {
      ret[root->left] = new Vertex{root->left->key};
      ret[root->left]->list.push_back(ret[root]);

      ret[root]->list.push_back(ret[root->left]);

      q.push(root->left);
    }

    if (root->right) {
      ret[root->right] = new Vertex{root->right->key};
      ret[root->right]->list.push_back(ret[root]);

      ret[root]->list.push_back(ret[root->right]);

      q.push(root->right);
    }

    q.pop();
  }

  return ret;
}

struct VertexComp {
  bool operator()(const Vertex* lhs, const Vertex* rhs) const {
    return lhs->data < rhs->data;
  }
};

using Path = set<Vertex*, VertexComp>;

int sum(Path* path) {
  int ret = 0;
  for (auto item : *path) {
    ret += item->data;
  }

  return ret;
}

struct PathComp {
  bool operator()(const Path* lhs, const Path* rhs) const {
    if (lhs->size() >= rhs->size()) {
      return false;
    }

    auto lIt = lhs->begin();
    auto rIt = rhs->begin();

    while (rIt != lhs->end()) {
      if ((*lIt)->data >= (*rIt)->data) {
        return false;
      }
    }

    return true;
  }
};

using Sums = set<Path*, PathComp>;

void bfs(int k, Vertex* startVertex, Sums& sums) {
  auto paths = map<Vertex*, Path*>();
  auto visited = map<Vertex*, bool>();

  auto q = queue<Vertex*>();
  q.push(startVertex);

  while (!q.empty()) {
    auto vertex = q.front();

    if (visited[vertex]) {
      q.pop();
      continue;
    }
    visited[vertex] = true;

    auto path = vertex->parent != nullptr
                    ? new Path(paths[vertex->parent]->begin(),
                               paths[vertex->parent]->end(), VertexComp())
                    : new Path();
    path->insert(vertex);

    paths[vertex] = path;

    if (sum(path) == k) {
      sums.insert(path);
    }

    for (auto item : vertex->list) {
      item->parent = vertex;
      q.push(item);
    }

    q.pop();
  }
}

int printCount(Node* root, int k) {
  auto g = toGraph(root);
  auto sums = Sums();

  for (auto item : g) {
    bfs(k, item.second, sums);
  }

  return sums.size();
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
  string testcases[] = {"1 3 L 3 2 L 3 -1 R -1 1 R"};

  int k = 4;

  for (auto item : testcases) {
    cout << printCount(parse(item), k) << endl;
  }

  return 0;
}