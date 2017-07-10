import java.util.*;

class GroupAnagrams {

    static class Word {
        HashMap<Character, Integer> map = new HashMap<>();

        Word(String raw) {
            initialize(raw);
        }

        void initialize(String raw) {
            if (raw == null) {
                return;
            }

            for (int i = 0; i < raw.length(); ++i) {
                char ch = raw.charAt(i);
                map.put(ch, map.containsKey(ch) ? map.get(ch) + 1 : 1);
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

    // public static void main(String[] args) {
    //     Map<Word, Integer> words = new HashMap<>();

    //     String[] inputs = new String[] { "act", "cat", "tac", "god", "dog" };

    //     for (String item : inputs) {
    // Word newItem = new Word(item);

    // words.put(newItem, words.containsKey(newItem) ? words.get(newItem) + 1 : 1);
    //     }

    //     Integer[] array = words.values().toArray(new Integer[] {});
    //     Arrays.sort(array);

    //     for (Integer i : array) {
    //         System.out.print(i + " ");
    //     }
    // }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        int T = scanner.nextInt();

        for (int i = 0; i < T; ++i) {
            Map<Word, Integer> words = new HashMap<>();

            int wordCount = scanner.nextInt();

            for (int j = 0; j < wordCount; ++j) {
                Word newItem = new Word(scanner.next());
                words.put(newItem, words.containsKey(newItem) ? words.get(newItem) + 1 : 1);
            }

            Integer[] array = words.values().toArray(new Integer[] {});
            Arrays.sort(array);

            for (Integer count : array) {
                System.out.print(count + " ");
            }
            System.out.print("\r\n");
        }
        scanner.close();
    }
}