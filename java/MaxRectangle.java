import java.util.*;
import java.lang.*;
import java.io.*;

// question : http://practice.geeksforgeeks.org/problems/maximum-sum-rectangle/0

class MaxRectangle {
  static class Sums {
    Map<String, Sum> map = new HashMap<>();

    void add(Sum sum) {
      map.put(sum.key(), sum);
    }

    Sum get(int x, int y) {
      return map.get(key(x, y));
    }

    public static String key(int x, int y) {
      return x + ":" + y;
    }
  }

  static class Sum implements Comparable<Sum> {
    int value, x, y;

    Sum(int x, int y) {
      this.x = x;
      this.y = y;
    }

    @Override
    public String toString() {
      return x + " : " + y + " = " + value;
    }

    @Override
    public int compareTo(Sum other) {
      return other.value - value;
    }

    public String key() {
      return Sums.key(x, y);
    }
  }

  public static void sum(Sums sums, int[] arr, int start, int end) {
    Sum ret = new Sum(start, end);

    if (end == 0) {
      ret.value = arr[end];
    } else {
      Sum prev = sums.get(start, end - 1);
      ret.value = (prev == null ? 0 : prev.value) + arr[end];
    }

    sums.add(ret);
    System.out.println(ret);
  }

  static Sums max1ds(int[] arr) {
    Sums sums = new Sums();

    for (int i = 0; i < arr.length; ++i) {
      for (int j = i; j < arr.length; ++j) {
        sum(sums, arr, i, j);
      }
    }

    return sums;
  }

  public static void main(String[] args) {
    int[][] array = new int[][] { { 1, 2, -1, 4, -20 }, { -8, -3, 4, 2, 1 }, { 3, 8, 10, -8, 3 },
        { -4, -1, 1, 7, -6 } };

    List<Sums> sumsList = new ArrayList<Sums>();
    for (int i = 0; i < array.length; ++i) {
      sumsList.add(max1ds(array[i]));
    }
  }
}