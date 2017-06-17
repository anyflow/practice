import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.ArrayList;
import java.util.Scanner;

class Trie {
  Node root = new Node((char) -1);

  class Node {
    char c;
    int size = 1;
    Map<Character, Node> children = new HashMap<Character, Node>();

    Node(char c) {
      this.c = c;
    }
  }

  void add(String contact) {
    Node node = root;
    for (int i = 0; i < contact.length(); ++i) {
      char c = contact.charAt(i);
      Node child = node.children.get(c);

      if (child == null) {
        child = new Node(c);
        node.children.put(c, child);
      } else {
        ++(child.size);
      }

      node = child;
    }
  }

  int count(String partial) {
    Node node = root;
    for (int i = 0; i < partial.length(); ++i) {
      node = node.children.get(partial.charAt(i));

      if (node == null) {
        return 0;
      }
    }

    return node.size;
  }

  static List<String> partials = new ArrayList<String>();

  public static void main(String[] args) {
    Trie trie = new Trie();

    trie.add("hack");
    trie.add("hackerrank");

    System.out.println(trie.count("hac"));
    System.out.println(trie.count("hak"));
  }

  // public static void main(String[] args) {
  //   Solution contacts = new Solution();

  //   Scanner scanner = new Scanner(System.in);

  //   int size = Integer.parseInt(scanner.next());

  //   for (int i = 0; i < size; ++i) {
  //     String operator = scanner.next();
  //     String operand = scanner.next();

  //     if (operator.equals("add")) {
  //         contacts.add(operand);
  //     } else {
  //         partials.add(operand);
  //     }
  //   }

  //   for(String partial : partials) {
  //     System.out.println(contacts.count(partial));
  //   }

  //   scanner.close();
  // }
}
