class Queue<T> {
  Node head = null;
  Node tail = null;

  void enqueue(T item) {
    Node nodeItem = new Node(item);
    if (head == null) {
      head = nodeItem;
      tail = head;
      return;
    }

    tail.next = nodeItem;
    tail = nodeItem;
  }

  T dequeue() {
    if (head == null) {
      return null;
    }

    Node ret = head;
    head = ret.next;
    if (head == null) {
      tail = null;
    }

    return ret.data;
  }

  T peek() {
    if (head == null) {
      return null;
    }
    return head.data;
  }

  class Node {
    T data;
    Node next;

    Node(T data) {
      this.data = data;
      next = null;
    }
  }

  public static void main(String[] args) {
    Queue<Integer> queue = new Queue<>();

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    Object value = null;

    do {
      value = queue.dequeue();
      System.out.println(value);
    } while (value != null);
  }
}