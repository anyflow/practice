import java.util.*;

class AnagramIndices {
  public static void main(String[] args) {
    String haystack = "abdcghbaabcdij";
    String needle = "bcda";

    List<Integer> ret = getAnagramIndices(haystack, needle);
    for (Integer i : ret) {
      System.out.println(i);
    }
  }

  static class Word {
    Map<Character, Integer> map = new HashMap<>();

    Word(String raw) {
      initialize(raw);
    }

    void initialize(String raw) {
      if (raw == null) {
        return;
      }

      for (int i = 0; i < raw.length(); ++i) {
        char ch = raw.charAt(i);
        map.put(ch, map.containsKey(ch) ? map.get(ch) : 1);
      }
    }

    @Override
    public int hashCode() {
      return map.hashCode();
    }

    @Override
    public String toString() {
      return map.toString();
    }

    @Override
    public boolean equals(Object other) {
      if (other == null) {
        return false;
      }

      Word word = (Word) other;
      if (map.size() != word.map.size()) {
        return false;
      }

      for (Character ch : map.keySet()) {
        if (map.get(ch) != word.map.get(ch)) {
          return false;
        }
      }
      return true;
    }
  }

  public static List<Integer> getAnagramIndices(String haystack, String needle) {
    List<Integer> ret = new ArrayList<Integer>();

    if (needle == null || haystack == null) {
      return ret;
    }

    Word needleWord = new Word(needle);
    for (int i = 0; i < haystack.length() - needle.length(); ++i) {
      int last = i + needle.length();
      System.out.println(haystack.substring(i, last));
      Word target = new Word(haystack.substring(i, last));
      if (needleWord.equals(target)) {
        ret.add(i);
      }
    }

    return ret;
  }
}