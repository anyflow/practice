import java.util.*;

class Partition {
  static Integer[] partition(int[] arr) {
    int pivot = arr[0];

    List<Integer> middle = new ArrayList<Integer>();
    middle.add(pivot);

    List<Integer> left = new ArrayList<Integer>();
    List<Integer> right = new ArrayList<Integer>();

    for (int i = 1; i < arr.length; ++i) {
      if(pivot == arr[i]) {
        middle.add(arr[i]);
      } else if(pivot > arr[i]) {
        left.add(arr[i]);
      } else {
        right.add(arr[i]);
      }
    }

    left.addAll(middle);
    left.addAll(right);

    return left.toArray(new Integer[0]);
  }

  static void print(Integer[] arr) {
    StringBuffer buf = new StringBuffer();

    for (int i : arr) {
      buf.append(i).append(" ");
    }

    String ret = buf.toString();
    System.out.println(arr.length > 0 ? ret.substring(0, ret.length() - 1) : "");
  }

  public static void main(String args[]) {
    Scanner in = new Scanner(System.in);

    int size = in.nextInt();
    int[] input = new int[size];
    for(int i=0; i<size; ++i) {
      input[i] = in.nextInt();
    }

    print(partition(input));
  }
}