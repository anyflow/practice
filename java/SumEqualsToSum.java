import java.util.*;

class SumEqualsToSum {
  static boolean exist(List<Integer> array) {
    Set<Integer> set = new HashSet<>();

    for (int i = 0; i < array.size(); ++i) {
      for (int j = i + 1; j < array.size(); ++j) {
        int target = array.get(i) + array.get(j);
        if (set.contains(target)) {
          return true;
        } else {
          set.add(target);
        }

      }
    }

    return false;
  }
  
  public static void main(String[] args) {
    Scanner scanner = new Scanner(System.in);

    int testcaseCount = scanner.nextInt();

    for (int i = 0; i < testcaseCount; ++i) {
      int arraySize = scanner.nextInt();

      List<Integer> list = new ArrayList<Integer>();

      for (int j = 0; j < arraySize; ++j) {
        list.add(scanner.nextInt());
      }

      System.out.println(exist(list) ? "1" : "0");
    }

    scanner.close();
  }
}