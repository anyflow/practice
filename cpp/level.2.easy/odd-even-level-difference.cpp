// https://practice.geeksforgeeks.org/problems/odd-even-level-difference/1

#include "../lib/practice.h"

using namespace std;

int getLevelDiff(Node* root) {
  if (root == nullptr) {
    return __INT32_MAX__;
  }

  int oddSum = 0;
  int evenSum = 0;

  struct Item {
    Node* node;
    bool isOdd;
  };

  auto que = queue<Item>();

  que.push(Item{root, true});

  while (que.size() > 0) {
    auto item = que.front();
    que.pop();

    item.isOdd ? oddSum += item.node->data : evenSum += item.node->data;

    if (item.node->left) {
      que.push({item.node->left, !item.isOdd});
    }

    if (item.node->right) {
      que.push({item.node->right, !item.isOdd});
    }
  }

  return oddSum - evenSum;
}

int main(int argc, char* argv[]) {
  int T;

  vector<string> treeNotations = {"1 2 R 1 3 L",
                                  "10 20 L 10 30 R 20 40 L 20 60 R"};
  auto trees = vector<Node*>();
  for (auto item : treeNotations) {
    trees.push_back(Tree::create(item));
  }

  for (auto tree : trees) {
    cout << getLevelDiff(tree) << endl;
    cout << endl;
  }

  return 0;
}