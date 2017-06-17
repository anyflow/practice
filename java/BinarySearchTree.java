import java.util.List;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Queue;

import javax.management.Query;

public class BinarySearchTree {

  class Nodes {
    Node current, parent;

    Nodes(Node current, Node parent) {
      this.current = current;
      this.parent = parent;
    }
  }

  class Node {
    int key;
    Node left, right;

    Node(int key) {
      this.key = key;
      left = null;
      right = null;
    }

    public String toString() {
      return Integer.toString(key);
    }

    void removeChild(int key) {
      if (left != null && left.key == key) {
        left = null;
      } else if (right != null && right.key == key) {
        right = null;
      }
    }
  }

  private Node root = null;

  public void insert(int key) throws Exception {
    root = insert(root, key);
  }

  public Node search(int key) {
    Nodes ret = search(root, null, key);

    return ret == null ? null : ret.current;
  }

  private Node insert(Node node, int key) throws Exception {
    if (node == null) {
      return new Node(key);
    }

    if (node.key > key) {
      node.left = insert(node.left, key);
    } else if (node.key < key) {
      node.right = insert(node.right, key);
    } else {
      throw new Exception("invalid key : " + Integer.toString(key));
    }

    return node;
  }

  private void attach(Node parent, Node node) throws Exception {
    if (parent == null) {
      throw new Exception("parent or node is null");
    }
    if (node == null) {
      return;
    }

    if (parent.key > node.key) {
      if (parent.left == null) {
        parent.left = node;
      } else {
        attach(parent.left, node);
      }
    } else if (parent.key < node.key) {
      if (parent.right == null) {
        parent.right = node;
      } else {
        attach(parent.right, node);
      }
    } else {
      throw new Exception("parent key is same with node key");
    }
  }

  public void delete(int key) throws Exception {
    Nodes nodes = search(root, null, key);

    if (nodes == null || nodes.current == null) {
      return;
    }

    Node current = nodes.current, parent = nodes.parent;

    if (current.left == null && current.right == null) {
      if (parent == null) {
        root = null;
        return;
      } else {
        parent.removeChild(key);
        return;
      }
    }

    Node newCurrent, counterpart;
    if (current.right != null) {
      newCurrent = current.right;
      counterpart = current.left;
    } else {
      newCurrent = current.left;
      counterpart = current.right;
    }

    if (parent != null) {
      if (parent.key > current.key) {
        parent.left = newCurrent;
      } else {
        parent.right = newCurrent;
      }
    } else {
      root = newCurrent;
    }

    attach(newCurrent, counterpart);
  }

  private Nodes search(Node current, Node parent, int key) {
    if (current.key > key) {
      return current.left == null ? null : search(current.left, current, key);
    } else if (current.key < key) {
      return current.right == null ? null : search(current.right, current, key);
    } else {
      return new Nodes(current, parent);
    }
  }

  public void inorderTraverse(Node node) {
    if (node.left != null) {
      inorderTraverse(node.left);
    }

    System.out.println(node.key);

    if (node.right != null) {
      inorderTraverse(node.right);
    }
  }

  public void preorderTraverse(Node node) {
    System.out.println(node.key);

    if (node.left != null) {
      preorderTraverse(node.left);
    }
    if (node.right != null) {
      preorderTraverse(node.right);
    }
  }

  public void postorderTraverse(Node node) {
    if (node.left != null) {
      postorderTraverse(node.left);
    }
    if (node.right != null) {
      postorderTraverse(node.right);
    }
    System.out.println(node.key);
  }

  public void levelorderTraverse(Node node) throws Exception {
    List<Node> start = new ArrayList<Node>();
    start.add(node);

    levelorderTraverse(start);
  }

  private void levelorderTraverse(List<Node> current) throws Exception {
    if (current == null) {
      throw new Exception("current cannot be null");
    }
    if (current.size() <= 0) {
      return;
    }

    List<Node> next = new ArrayList<Node>();
    for (Node item : current) {
      System.out.println(item.key);

      if (item.left != null) {
        next.add(item.left);
      }
      if (item.right != null) {
        next.add(item.right);
      }
    }

    levelorderTraverse(next);
  }

  public Node root() {
    return root;
  }

  public static void main(String[] args) {
    /* Let us create following BST
          50
       /     \
      30      70
     /  \    /  \
    20   40  60   80 */

    BinarySearchTree bst = new BinarySearchTree();

    try {
      bst.insert(50);
      bst.insert(30);
      bst.insert(70);
      bst.insert(20);
      bst.insert(40);
      bst.insert(60);
      bst.insert(80);

      System.out.println("inorder:");
      bst.inorderTraverse(bst.root());
      System.out.println("preorder:");
      bst.preorderTraverse(bst.root());
      System.out.println("postorder:");
      bst.postorderTraverse(bst.root());
      System.out.println("levelorder:");
      bst.levelorderTraverse(bst.root());

      bst.delete(50);
      bst.delete(40);
      bst.delete(60);

      System.out.println("after delete 50, 40, 60");
      bst.inorderTraverse(bst.root());
    } catch (Exception e) {
      System.out.println(e);
      return;
    }

    System.out.println(bst.search(20));
  }
}