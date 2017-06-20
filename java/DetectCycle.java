class DetectCycle {

  class Node {
    int data;
    Node next;
  }

  boolean hasCycle(Node head) {
    Node one = head;
    Node two = head;

    if(two.next == null) { return false; }
    two = two.next.next;
    one = one.next;

    while(two != null) {
      if(one.data == two.data) {
        return true;
      }

      if(two.next == null) { break; }
      two = two.next.next;
      one = one.next;
    }

    return false;
  }

  static void main(String[] args) {

  }
}