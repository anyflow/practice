import java.util.*;

class MinimumSquares {
  static class Item implements Comparable<Item> {
    char ch;
    int count;

    Item(char ch) {
      this.ch = ch;
      this.count = 1;
    }

    @Override
    public boolean equals(Object o) {
      Item other = (Item) o;

      return other != null && other.ch == this.ch;
    }

    @Override
    public int hashCode() {
      return Character.hashCode(ch);
    }

    @Override
    public int compareTo(Item other) {
      return other.count - count;
    }

    @Override
    public String toString() {
      return ch + ":" + count;
    }
  }

  static List<Item> get(String testCase) {
    Map<Character, Item> map = new TreeMap<>();

    if (testCase == null || testCase.isEmpty()) {
      return new ArrayList<Item>();
    }

    for (char ch : testCase.toCharArray()) {
      if (map.containsKey(ch)) {
        ++(map.get(ch).count);
      } else {
        map.put(ch, new Item(ch));
      }
    }

    return new ArrayList<Item>(map.values());
  }

  static List<Item> removeCounts(List<Item> list, int removalCount) {
    while (removalCount > 0) {
      list.sort(null);

      --(list.get(0).count);
      --removalCount;
    }

    return list;
  }

  static void print(List<Item> list) {
    class Temp {
      int ret = 0;
    }
    final Temp temp = new Temp();
    list.forEach(item -> temp.ret += Math.pow(item.count, 2));

    System.out.println(temp.ret);
  }

  public static void main(String[] args) {
    String testcase = "abccc";
    int removalCount = 1;

    print(removeCounts(get(testcase), removalCount));

    Set<Integer> s;
  }
}