import java.util.HashMap;
import java.util.Map;

class MakingAnagrams {
  int countToRemove(String first, String second) {
    int[] charCounts = new int[26];
    for (char ch : first.toCharArray()) {
      charCounts[ch - 'a']++;
    }
    for (char ch : second.toCharArray()) {
      charCounts[ch - 'a']--;
    }

    int ret = 0;
    for(int diff : charCounts) {
      ret += Math.abs(diff);
    }

    return ret;
  }

  public static void main(String[] args) {
    String first = "fcrxzwscanmligyxyvym";
    String second = "jxwtrhvujlmrpdoqbisbwhmgpmeoke";

    System.out.println(new MakingAnagrams().countToRemove(first, second));
  }
}