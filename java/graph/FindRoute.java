import java.util.*;

class FindRoute {
  static class Cell {
    int x, y;
    Cell parent;

    Cell(int x, int y) {
      this(x, y, null);
    }

    Cell(int x, int y, Cell parent) {
      this.x = x;
      this.y = y;
      this.parent = parent;
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

  static void BFS(int[][] matrix, boolean[][] visited, Cell start, Cell end) {
    Queue<Cell> toVisit = new ArrayDeque<>();
    toVisit.add(start);

    while (toVisit.peek() != null) {
      Cell target = toVisit.poll();
      visited[target.x][target.y] = true;

      if (target.equals(end)) {
        end.parent = target.parent;
        return;
      }

      for (int x = -1; x < 2; ++x) {
        for (int y = -1; y < 2; ++y) {
          Cell cell = new Cell(target.x + x, target.y + y);

          if (cell.x < 0 || cell.x >= matrix.length || cell.y < 0 || cell.y >= matrix[0].length) {
            continue;
          }

          if (matrix[cell.x][cell.y] != 1 || visited[cell.x][cell.y]) {
            continue;
          }

          toVisit.add(cell);
          cell.parent = target;
        }
      }
    }
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

    visited = new boolean[matrix.length][matrix[0].length];
    BFS(matrix, visited, start, end);

    System.out.print("via BFS : ");
    Cell cell = end;
    while (cell != null) {
      System.out.print(cell + " - ");
      cell = cell.parent;
    }
  }
}