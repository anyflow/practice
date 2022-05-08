import java.util.*;

class SortedList {
  List<Integer> list = new ArrayList<>();

  public void add(int e) {
    Integer index = findIndex(e, 0, list.size());
    if(index == null) { return; }
    list.add(index, e);
  }

  Integer findIndex(int n, int from, int to) {
    if (to > list.size() || from < 0 || to - from < 0) {
      return null;
    }

    if (list.size() == 0) {
      return 0;
    }

    int midIndex = (int) Math.floor((from + to) / 2);
    int mid = list.get(midIndex);

    if (mid == n) {
      return midIndex;
    } else if (n < mid) {
      return to - from == 1 ? midIndex : findIndex(n, from, midIndex);
    } else {
      return to - from == 1 ? midIndex + 1 : findIndex(n, midIndex, to);
    }
  }

  @Override
  public String toString() {
    return list.toString();
  }

  public static void main(String[] args) {
    int[] array = new int[] { 100, 50, 60, 40, 80, 30, 70, 20, 90, 10 };
    SortedList sortedList = new SortedList();

    for (Integer i : array) {
      sortedList.add(i);
    }

    try {
      int toInsert = 30;
      sortedList.add(toInsert);

      System.out.println(sortedList);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}