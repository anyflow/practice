import java.util.PriorityQueue;

class Temp {
  public static void main(String[] args) {
    PriorityQueue<Integer> queue = new PriorityQueue<>();

    queue.add(11);
    queue.add(2);
    queue.add(34);

    while (queue.size() != 0) {
      System.out.println(queue.poll());
    }
  }
}