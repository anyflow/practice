#include "../common/practice.h"

using namespace std;

bool isIsomorphic(Node* root1, Node* root2) {
  if (root1 == nullptr && root2 == nullptr) {
    return true;
  }
  if (root1 == nullptr || root2 == nullptr) {
    return false;
  }

  if (root1->data != root2->data) {
    return false;
  }

  return (isIsomorphic(root1->left, root2->left) &&
          isIsomorphic(root1->right, root2->right)) ||
         (isIsomorphic(root1->left, root2->right) &&
          isIsomorphic(root1->right, root2->left));
}

int main(int argc, char* argv[]) {
  //   auto tree1 = parse(string("1 2 L 1 3 R 2 4 L"));
  //   auto tree2 = parse(string("1 3 L 1 2 R 3 4 R"));

  //   auto tree1 = parse(string("1 2 L 1 3 R 2 4 L"));
  //   auto tree2 = parse(string("1 3 L 1 2 R 2 4 R"));

  auto tree1 = TreeFactory::create(string("0 1 L 0 2 R"));
  auto tree2 = TreeFactory::create(string("0 1 L 0 2 R"));

  string ret = isIsomorphic(tree1, tree2) ? "YES" : "NO";
  cout << ret << endl;

  return 0;
}