import java.util.*;
import java.lang.*;
import java.io.*;

// question : http://practice.geeksforgeeks.org/problems/maximum-sum-rectangle/0

class MaxRectangle {
  static class Sums {
    Map<String, Sum> map = new HashMap<>();
    Set<Sum> set = new TreeSet<>();

    void add(Sum sum) {
      map.put(sum.key(), sum);
      set.add(sum);
    }

    Sum get(int hStart, int hEnd, int vStart, int vEnd) {
      return map.get(key(hStart, hEnd, vStart, vEnd));
    }

    public static String key(int hStart, int hEnd, int vStart, int vEnd) {
      return hStart + ":" + hEnd + ":" + vStart + ":" + vEnd;
    }
  }

  static class Sum implements Comparable<Sum> {
    int value, hStart, hEnd, vStart, vEnd;

    Sum(int hStart, int hEnd, int vIndex) {
      this.hStart = hStart;
      this.hEnd = hEnd;
      this.vStart = this.vEnd = vIndex;
    }

    Sum(int hStart, int hEnd, int vStart, int vEnd) {
      this.hStart = hStart;
      this.hEnd = hEnd;
      this.vStart = vStart;
      this.vEnd = vEnd;
    }

    @Override
    public String toString() {
      return vStart + " : " + vEnd + " : " + hStart + " : " + hEnd + " = " + value;
    }

    @Override
    public int compareTo(Sum other) {
      return other.value - value;
    }

    public String key() {
      return Sums.key(hStart, hEnd, vStart, vEnd);
    }

    @Override
    public boolean equals(Object o) {
      Sum other = (Sum) o;

      return hStart == other.hStart && hEnd == other.hEnd && vStart == other.vStart && vEnd == other.vEnd;
    }
  }

  public static void sum(Sums sums, int[][] arr, int hStart, int hEnd, int vStart, int vEnd) {
    Sum ret = new Sum(hStart, hEnd, vStart, vEnd);

    int lefterPrev = 0;
    if (hStart == hEnd) {
      lefterPrev = 0;
    } else {
      int prevAll = sums.get(hStart, hEnd - 1, vStart, vEnd).value;
      lefterPrev = prevAll - (vStart == vEnd ? 0 : sums.get(hStart, hEnd - 1, vStart, vEnd - 1).value);
    }

    int upperPrev = vStart == vEnd ? 0 : sums.get(hStart, hEnd, vStart, vEnd - 1).value;

    ret.value = lefterPrev + upperPrev + arr[vEnd][hEnd];

    sums.add(ret);
    System.out.println(ret);
  }

  public static void main(String[] args) {
    try {
      int[][] array = new int[][] { { 1, 2, -1, 4, -20 }, { -8, -3, 4, 2, 1 }, { 3, 8, 10, -8, 3 },
          { -4, -1, 1, 7, -6 } };

      Sums sums = new Sums();

      for (int vStart = 0; vStart < array.length; ++vStart) {
        for (int vEnd = vStart; vEnd < array.length; ++vEnd) {
          for (int hStart = 0; hStart < array[0].length; ++hStart) {
            for (int hEnd = hStart; hEnd < array[0].length; ++hEnd) {
              sum(sums, array, hStart, hEnd, vStart, vEnd);
            }
          }
        }
      }

      System.out.println("max : " + sums.set.iterator().next());
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}