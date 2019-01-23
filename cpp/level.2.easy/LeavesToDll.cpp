// TODO
// https://practice.geeksforgeeks.org/problems/leaves-to-dll/1

#include "../lib/practice.h"

using namespace std;
using namespace anyflow;

Node* last = nullptr;

Node* traverse(Node* node, Node** head_ref) {
  if (node->left) {
    traverse(node->left, head_ref);
  }

  if (!node->left && !node->right) {
    auto leaf = new Node{node->data, last, nullptr};

    if (last == nullptr) {
      last = leaf;
      *head_ref = last;
    } else {
      last->right = leaf;
      last = last->right;
    }
  }

  if (node->right) {
    traverse(node->right, head_ref);
  }

  return last;
}

Node* convertToDLL(Node* root, Node** head_ref) {
  if (root == nullptr) {
    return nullptr;
  }

  return traverse(root, head_ref);
}

int main(int argc, char* argv[]) {
  auto tree = Tree::create("1 2 L 1 3 R 2 4 L");

  Node* dll = new Node{0, nullptr, nullptr};

  auto last = convertToDLL(tree, &dll);

  cout << "data : " << last->data << endl
       << "left : " << last->left->data << endl
       << "right : " << (last->right ? to_string(last->right->data) : "null")
       << endl;

  return 0;
}