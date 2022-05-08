public class Stack {
  private Node top;

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

  public Object pop() {
    if (top == null) {
      return null;
    }

    Object ret = top.value();
    top = top.next();

    return ret;
  }

  public void push(Object value) {
    Node node = new Node(value);

    if (top == null) {
      top = node;
    } else {
      node.setNext(top);
      top = node;
    }
  }

  public static void main(String[] args) {
    Stack stack = new Stack();

    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);

    Object value = null;

    do {
      value = stack.pop();

      System.out.println(value);
    } while (value != null);

    return;
  }
}