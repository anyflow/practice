class CheckBst {
  class Node {
    int data;
    Node left;
    Node right;
  }

  Integer last = null;
  boolean checkBST(Node node) {
    if(node == null) { return true; }

    if (!checkBST(node.left)) {
      return false;
    }

    if(last != null && last >= node.data) {
      return false;
    }
    last = node.data;

    if (!checkBST(node.right)) {
      return false;
    }

    return true;
  }
}