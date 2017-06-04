import java.util.Arrays;

class InsertionSort {
  static void sort(int[] arr) {
    int temp = 0, j = 0;

    for (int i = 1; i < arr.length; ++i) {
      temp = arr[i];
      j = i - 1;

      while(j >= 0 && arr[j] > temp) {
        arr[j + 1] = arr[j];
        --j;
      }

      arr[j + 1] = temp;
    }
  }

  public static void main(String args[]) {
    int[] input = new int[] { 3, 1, 5, 9, 0, 10, 5, 32, -1, -20, 239, 583, 6, 3, 54, 53, 38 };

    System.out.println("before(" + input.length + ") : " + Arrays.toString(input));
    
    sort(input);

    System.out.println("after(" + input.length + ")  : " + Arrays.toString(input));
  }
}