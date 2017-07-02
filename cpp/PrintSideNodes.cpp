// question :
// http://practice.geeksforgeeks.org/problems/leftmost-and-rightmost-nodes-of-binary-tree/1

#include <iostream>
#include <map>
#include <queue>
#include <string>

using namespace std;

class Node {
public:
  int key;
  Node* left;
  Node* right;

  Node(int key) {
    this->key = key;
    left = nullptr;
    right = nullptr;
  }

  string toString() { return std::to_string(key); }
};

class Most {
public:
  bool leftMost;
  bool rightMost;

  Most() : leftMost(false), rightMost(false) {}
  Most(bool leftMost, bool rightMost)
      : leftMost(leftMost), rightMost(rightMost) {}
};

void printCorner(Node* root) {
  std::queue<Node*> queue;
  std::map<Node*, Most> map;

  queue.push(root);
  map[root] = Most(true, true);

  while (queue.front() != nullptr) {
    Node* node = queue.front();

    if (map[node].leftMost || map[node].rightMost) {
      cout << node->key << " ";
    }

    if (node->left != nullptr) {
      if (map[node].leftMost) {
        queue.push(node->left);
        map[node->left] = Most(true, false);
      }
    }

    if (node->right != nullptr) {
      if (map[node].rightMost) {
        queue.push(node->right);
        map[node->right] = Most(false, true);
      }
    }

    queue.pop();
  }
}

int main(int argc, char* argv[]) {
  Node* head = new Node(15);
  head->left = new Node(10);
  head->left->left = new Node(8);
  head->left->right = new Node(12);
  head->right = new Node(20);
  head->right->left = new Node(16);
  head->right->right = new Node(25);

  printCorner(head);

  return 0;
}