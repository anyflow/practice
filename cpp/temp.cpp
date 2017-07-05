#include <algorithm>
#include <climits>
#include <iostream>
#include <string>

using namespace std;

auto t = [](auto a, auto b) { return a + b; };

struct Node {
  int data;
};

struct NodeEx : public Node {
  Node* parent;
  // NodeEx(Node* node) : data(node->data), left(node->left), right(node->right)
  // {}
  NodeEx() {}
  NodeEx(Node* node, Node* parent) : Node{node->data}, parent(parent) {}
};

int main() {
  auto node = new Node{11};
  auto n1 = new NodeEx(node, nullptr);

  cout << node->data << endl;
}