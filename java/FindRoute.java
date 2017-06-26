import java.util.*;

//question : https://www.hackerrank.com/challenges/ctci-connected-cell-in-a-grid

class FindRoute {
  static class Cell {
    int x, y;

    Cell(int x, int y) {
      this.x = x;
      this.y = y;
    }

    @Override
    public String toString() {
      return x + ":" + y;
    }

    @Override
    public boolean equals(Object o) {
      Cell other = (Cell) o;
      if (other == null) {
        return false;
      }

      return x == other.x && y == other.y;
    }
  }

  static boolean DFS(int[][] matrix, boolean[][] visited, List<Cell> route, Cell start, Cell end) {
    if (start.x < 0 || start.x >= matrix.length || start.y < 0 || start.y >= matrix[0].length) {
      return false;
    }
    if (matrix[start.x][start.y] != 1 || visited[start.x][start.y]) {
      return false;
    }

    visited[start.x][start.y] = true;

    if (start.equals(end)) {
      route.add(start);
      return true;
    }

    for (int x = -1; x < 2; ++x) {
      for (int y = -1; y < 2; ++y) {
        if (x == 0 && y == 0) {
          continue;
        }

        if (DFS(matrix, visited, route, new Cell(start.x + x, start.y + y), end)) {
          route.add(start);
          return true;
        }
      }
    }

    return false;
  }

  static boolean BFS(int[][] matrix, boolean[][] visited, List<Cell> route, Cell start, Cell end) {
    return false;
  }

  public static void main(String[] args) {
    // 1 0 0 0 0 0
    // 0 1 1 1 0 0
    // 0 1 0 1 0 0
    // 1 1 0 1 0 0
    // 1 0 0 0 1 1

    int[][] matrix = { { 1, 0, 0, 0, 0, 0 }, { 0, 1, 1, 1, 0, 0 }, { 1, 1, 0, 1, 0, 0 }, { 1, 1, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1 } };
    boolean[][] visited = new boolean[matrix.length][matrix[0].length];

    Cell start = new Cell(0, 0);
    Cell end = new Cell(4, 5);

    List<Cell> route = new ArrayList<>();

    DFS(matrix, visited, route, start, end);

    System.out.print("via DFS : ");
    route.forEach(i -> System.out.print(i + " - "));
    System.out.println();

    route = new ArrayList<>();

    BFS(matrix, visited, route, start, end);

    System.out.print("via BFS : ");
    route.forEach(i -> System.out.print(i + " - "));
  }
}