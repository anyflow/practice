package java;

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