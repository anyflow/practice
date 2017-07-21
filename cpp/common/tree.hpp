#ifndef TREE_HPP
#define TREE_HPP

#include <sstream>
#include <string>
#include <vector>

struct Node {
  int data;
  Node* left;
  Node* right;
};

class TreeFactory {
public:
  static Node* create(std::string treeNotation) {
    using namespace std;

    stringstream stream(treeNotation);
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
  };

private:
  static void insert(Node* root, int n1, int n2, char lr) {
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
  };
};

#endif // TREE_HPP