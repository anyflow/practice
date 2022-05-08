// https://practice.geeksforgeeks.org/problems/construct-tree-from-preorder-traversal/1
// Ressult : OK

#include "../lib/practice.h"

using namespace std;
using namespace anyflow;

Node* constructTree(int n, int pre[], char preLN[]);

int main(int argc, char* argv[]) {
  int pre[] = {10, 30, 20, 5, 15};
  char preLN[] = {'N', 'N', 'L', 'L', 'L'};

  int size = sizeof(pre) / sizeof(pre[0]);

  auto tree = constructTree(size, pre, preLN);

  anyflow::Tree::printPaths(tree);

  return 0;
}

Node* constructInner(int size, int curPos, int pre[], char preLN[],
                     stack<Node*>& stack) {
  if (curPos >= size) {
    return nullptr;
  }

  auto curr = new Node{pre[curPos], nullptr, nullptr};

  if (!stack.empty()) {
    auto parent = stack.top();

    if (parent->left == nullptr) {
      parent->left = curr;
    } else {
      parent->right = curr;
      stack.pop();
    }
  }

  if (preLN[curPos] == 'N') {
    stack.push(curr);
  }

  constructInner(size, curPos + 1, pre, preLN, stack);

  return curr;
}

Node* constructTree(int n, int pre[], char preLN[]) {
  stack<Node*> stack;

  return constructInner(n, 0, pre, preLN, stack);
}