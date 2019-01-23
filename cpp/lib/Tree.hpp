#ifndef TREE_HPP
#define TREE_HPP

#include <sstream>
#include <string>
#include <vector>

namespace anyflow {

struct Node {
  int data;
  Node* left;
  Node* right;
};

class Tree {
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

  static void printPaths(Node* root) {
    printPathsInner(root, std::vector<int>());
    std::cout << std::endl;
  }

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

  static void printPathsInner(Node* root, std::vector<int> path) {
    if (root == nullptr) {
      return;
    }

    path.push_back(root->data);

    if (!root->left && !root->right) {
      for (auto i : path) {
        std::cout << i << " ";
      }
      std::cout << "#";
      return;
    }

    if (root->left) {
      printPathsInner(root->left, path);
    }
    if (root->right) {
      printPathsInner(root->right, path);
    }
  }
};

} // namespace anyflow

#endif // TREE_HPP