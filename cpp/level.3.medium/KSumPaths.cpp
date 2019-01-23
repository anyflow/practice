// https://practice.geeksforgeeks.org/problems/k-sum-paths/1

#include "../lib/practice.h"

using namespace std;
using namespace anyflow;

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
  auto parents = map<Node*, Node*>();

  auto q = queue<Node*>();
  q.push(startNode);

  while (!q.empty()) {
    auto node = q.front();

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

int main(int argc, char* argv[]) {
  auto testcases = map<int, string>();
  testcases[4] = "1 3 L 3 2 L 3 -1 R -1 1 R";
  testcases[3] = "0 1 L 0 2 R 1 3 L 1 4 R 2 5 L 2 6 R";

  for (auto item : testcases) {
    cout << "k=" << item.first << "|tree:" << item.second << endl;
    cout << printCount(Tree::create(item.second), item.first) << endl;
  }

  return 0;
}