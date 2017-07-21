// http://practice.geeksforgeeks.org/problems/leftmost-and-rightmost-nodes-of-binary-tree/1

#include "../common/practice.h"

using namespace std;

void printCornerInner(vector<Node*>& nodes, string& ret) {
  if (nodes.size() <= 0) {
    return;
  }

  vector<Node*> nextNodes;
  for (auto it = nodes.begin(); it != nodes.end(); ++it) {
    if (it == nodes.begin() || it == nodes.end() - 1) {
      cout << (*it)->data << " ";
      ret.append(to_string((*it)->data));
      ret.append(" ");
    }

    if ((*it)->left != nullptr) {
      nextNodes.push_back((*it)->left);
    }
    if ((*it)->right != nullptr) {
      nextNodes.push_back((*it)->right);
    }
  }

  printCornerInner(nextNodes, ret);
}

string printCorner(Node* root) {
  string ret;

  if (root == nullptr) {
    return ret;
  }

  vector<Node*> nextNodes;
  nextNodes.push_back(root);

  printCornerInner(nextNodes, ret);
}

int main(int argc, char* argv[]) {
  string testcases[] = {"15 10 L 10 8 L 10 12 R 15 20 R 20 16 L 20 25 R",
                        "4 1 L 4 2 R 1 6 L 1 10 R 2 2 L 2 3 R 6 9 L 6 1 R 10 "
                        "10 L 10 3 R 2 5 L 2 10 R 3 1 L 3 10 R"};

  for (auto item : testcases) {
    string ret = printCorner(TreeFactory::create(item));
    cout << endl;
  }

  return 0;
}