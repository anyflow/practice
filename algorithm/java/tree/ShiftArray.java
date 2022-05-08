public class ShiftArray {
  Node left;
  Node right;

  class Node {
    int data;
    Node next;

    Node(int data) {
      this.data = data;
    }
  }

  void shiftAndPrint(int[] array, int count) {
    if(array == null || array.length <= 0) {
      return;
    }

    for(int i = 0; i < array.length; ++i) {
      Node node = new Node(array[i]);
      if(left == null) {
        left = node;
        right = node;
        continue;
      }

      right.next = node;
      right = node;
    }

    for(int i = 0; i < count; ++i) {
      right.next = left;
      left = left.next;
      right = right.next;
      right.next = null;
    }

    Node itr = left;
    while (itr != null) {
      System.out.print(itr.data + " ");
      itr = itr.next;
    }
  }

  public static void main(String[] args) {
    int[] array = new int[] { 1, 2, 3, 4, 5 };

    (new ShiftArray()).shiftAndPrint(array, 2);
  }
}