// https://practice.geeksforgeeks.org/problems/height-of-spiral-tree/1/?ref=self
// Ressult : OK

#include "../lib/practice.h"

using namespace std;
using namespace anyflow;

int findTreeHeight(Node* node);

int main(int argc, char* argv[]) {
  auto tree = anyflow::Tree::create("1 2 L 1 3 R 2 4 L 2 5 R 4 6 L");

  cout << findTreeHeight(tree) << endl;

  return 0;
}

struct Item {
  Node* node;
  int level;

  int data() { return node->data; }
  Node* left() { return node->left; }
  Node* right() { return node->right; }
};

bool isLeaf(Item& item) {
  // return item.right() != nullptr && item.node == (item.right()->left);
  return item.left() == nullptr && item.right() == nullptr;
}

int findTreeHeight(Node* node) {
  queue<Item> queue;

  queue.push(Item{node, 1});

  int height = 0;
  while (!queue.empty()) {
    auto item = queue.front();
    queue.pop();

    height = item.level;

    if (isLeaf(item)) {
      continue;
    }

    if (item.left() != nullptr)
      queue.push(Item{item.left(), item.level + 1});
    if (item.right() != nullptr)
      queue.push(Item{item.right(), item.level + 1});
  }

  return height;
}
