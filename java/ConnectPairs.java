import java.util.*;

class ConnectPairs {

  public static void main(String[] args) {
    String[][] input = new String[][] { {"1", "2"}, {"2", "3"}, {"4", "5"}};

    Temp2 temp2 = new Temp2();

    String[] ret = temp2.largestItemAssociation(input);

    System.out.println(ret);
  }

  public Set<String> toHashSet(String[] target) {
    Set<String> ret = new HashSet<>();
    if (target == null) {
      return ret;
    }

    for (String i : target) {
      ret.add(i);
    }

    return ret;
  }

  public void merge(Set<String> target, String[] items) {
    boolean mergable = false;
    for(String i : items) {
      if(target.contains(i)) {
        mergable = true;
        break;
      }
    }

    if(mergable == false) { return; }
    for(String i : items) {
      target.add(i);
    }
  }

  public static Comparator<Set<String>> comp = new Comparator<Set<String>>() {
    public int compare(Set<String> first, Set<String> second) {
      if(first.size() < second.size()) {
        return -1;
      } else if(first.size() > second.size()) {
        return 1;
      }

      return 0;
    }
  };
  public String[] largestItemAssociation(String[][] itemAssociation) {
    if (itemAssociation == null) {
      return null;
    }

    List<Set<String>> sets = new ArrayList<>();

    for (int i = 0; i < itemAssociation.length; ++i) {
      Set<String> item = toHashSet(itemAssociation[i]);
      for (int j = i + 1; j < itemAssociation.length; ++j) {
        merge(item, itemAssociation[j]);
      }

      sets.add(item);
    }

    sets.sort(comp);

    Set<String> ret2 = sets.get(sets.size() - 1);
    String[] ret = ret2.toArray(new String[] {});
    Arrays.sort(ret);

    String t;
    return ret;
  }
}