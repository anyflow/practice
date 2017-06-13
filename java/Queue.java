public class Queue {
  private Node left = null;
  private Node right = null;

  public class Node {
    private Object value;
    private Node next;

    public Node(Object value) {
      this.value = value;
    }

    public Object value() {
      return value;
    }

    public Node next() {
      return next;
    }

    public void setNext(Node node) {
      this.next = node;
    }
  }

  public void enqueue(Object value) {
    Node node = new Node(value);

    Node temp = right;
    if (temp != null) {
      temp.setNext(node);
      right = node;
    } else {
      if (left == null) {
        left = node;
      } else {
        right = node;
        left.setNext(right);
      }
    }
  }

  public Object dequeue() {
    if (left == null) {
      return null;
    }

    Node ret = left;
    left = left.next();

    return ret.value();
  }

  public static void main(String[] args) {
    Queue queue = new Queue();

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);

    Object value = null;

    do {
      value = queue.dequeue();
      System.out.println(value);
    } while(value != null);
  }
}