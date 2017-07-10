import java.util.*;

//question : http://practice.geeksforgeeks.org/problems/count-the-elements/0

class CountElements {

    public static void main(String[] args) {
        int[] arrA = new int[] { 1, 2, 3, 4, 7, 9 };
        int[] arrB = new int[] { 0, 1, 2, 1, 1, 4 };
        // int[] arrA = new int[] { 95, 39, 49, 20, 67, 26, 63 };
        // int[] arrB = new int[] { 77, 96, 81, 65, 60, 36, 55 };

        Map<Integer, Integer> map = new TreeMap<>();

        List<Integer> A = new ArrayList<>();
        List<Integer> B = new ArrayList<>();

        for (int a : arrA) {
            A.add(a);
            map.put(a, 0);
        }
        for (int b : arrB) {
            B.add(b);
        }

        B.sort(null);

        Iterator<Integer> itrA = map.keySet().iterator();
        int j = 0;
        int prev = 0;
        while (itrA.hasNext()) {
            int a = itrA.next();
            map.put(a, prev);

            for (; j < B.size(); ++j) {
                int b = B.get(j);

                if (b <= a) {
                    int size = map.get(a);
                    map.put(a, ++size);
                } else {
                    break;
                }
            }
            prev = map.get(a);
        }

        StringBuffer buf = new StringBuffer();
        for (int a : A) {
            buf = buf.append(map.get(a)).append(',');
        }

        String ret = buf.toString();
        ret = map.size() > 0 ? ret.substring(0, ret.length() - 1) : ret;

        System.out.println(ret);
    }
}