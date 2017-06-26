import java.util.*;

class ConnectedCells {
  static int countCells(int[][] matrix, boolean[][] visited, int x, int y) {
    if (x < 0 || x >= matrix.length || y < 0 || y >= matrix[0].length) {
      return 0;
    }
    if (matrix[x][y] == 0 || visited[x][y]) {
      return 0;
    }

    visited[x][y] = true;

    int ret = 0;

    ret += countCells(matrix, visited, x - 1, y - 1);
    ret += countCells(matrix, visited, x - 1, y);
    ret += countCells(matrix, visited, x - 1, y + 1);
    ret += countCells(matrix, visited, x, y - 1);
    ret += countCells(matrix, visited, x, y + 1);
    ret += countCells(matrix, visited, x + 1, y - 1);
    ret += countCells(matrix, visited, x + 1, y);
    ret += countCells(matrix, visited, x + 1, y + 1);

    return ret;
  }

  public static void main(String[] args) {
    int[][] matrix = { { 1, 1, 1, 0, 1 }, { 0, 0, 1, 0, 0 }, { 1, 1, 0, 1, 0 }, { 0, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 0 }, { 0, 0, 1, 1, 0 } };

    boolean[][] visited = new boolean[matrix.length][matrix[0].length];
    int max = 0;
    for (int x = 0; x < matrix[0].length; ++x) {
      for (int y = 0; y < matrix.length; ++y) {
        max = Math.max(max, countCells(matrix, visited, x, y));
      }
    }

    System.out.println(max);
  }
}