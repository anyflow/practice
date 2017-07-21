// http://practice.geeksforgeeks.org/problems/root-to-leaf-paths/1

#include "../common/practice.h"

using namespace std;

void printPathsInner(Node* root, vector<int> path) {
  if (root == nullptr) {
    return;
  }

  path.push_back(root->data);

  if (!root->left && !root->right) {
    for (auto i : path) {
      cout << i << " ";
    }
    cout << "#";
    return;
  }

  if (root->left) {
    printPathsInner(root->left, path);
  }
  if (root->right) {
    printPathsInner(root->right, path);
  }
}

void printPaths(Node* root) {
  printPathsInner(root, vector<int>());
  cout << endl;
}

int main(int argc, char* argv[]) {
  auto testcases =
      vector<string>{"1 2 R 1 3 L", "10 20 L 10 30 R 20 40 L 20 60 R",
                     "1 2 L 1 3 R 2 4 L 2 5 R 3 6 L 3 7 R"};

  for (auto item : testcases) {
    printPaths(TreeFactory::create(item));
    cout << endl;
  }

  return 0;
}