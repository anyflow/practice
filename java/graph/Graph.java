import java.util.*;

class Graph<V> {
  Map<V, LinkedList<V>> map = new HashMap<>();

  void addEdge(V sourceVertex, V targetVertex) {
    LinkedList<V> list = map.get(sourceVertex);
    if (list == null) {
      list = new LinkedList<>();
      map.put(sourceVertex, list);
    }

    list.add(targetVertex);
  }

  void BFS(V startVertex) {
    LinkedList<V> list = map.get(startVertex);
    if (list == null) {
      return;
    }

    Set<V> visited = new HashSet<>();

    java.util.Queue<V> queue = new ArrayDeque<>();
    queue.add(startVertex);
    visited.add(startVertex);

    while (queue.peek() != null) {
      startVertex = queue.poll();

      System.out.print(startVertex + " ");

      list = map.get(startVertex);

      ListIterator<V> itr = list.listIterator();
      while (itr.hasNext()) {
        V vertex = itr.next();

        if (visited.contains(vertex)) {
          continue;
        }

        queue.add(vertex);
        visited.add(vertex);
      }
    }
  }

  void DFS(V startVertex) {
    Set<V> visited = new HashSet<>();

    DFSInner(startVertex, visited);
  }

  void DFSInner(V startVertex, Set<V> visited) {
    LinkedList<V> list = map.get(startVertex);
    if (list == null) {
      return;
    }

    visited.add(startVertex);
    System.out.print(startVertex + " ");

    ListIterator<V> itr = list.listIterator();
    while (itr.hasNext()) {
      V vertex = itr.next();

      if(visited.contains(vertex)) { continue; }

      DFSInner(vertex, visited);
    }
  }

  public static void main(String args[]) {
    // 0 - 1 \
    // |   |   2
    // 4 - 3 /

    Graph<Integer> graph = new Graph<>();

    graph.addEdge(0, 1);
    graph.addEdge(0, 4);
    graph.addEdge(1, 0);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 1);
    graph.addEdge(2, 3);
    graph.addEdge(3, 1);
    graph.addEdge(3, 2);
    graph.addEdge(3, 4);
    graph.addEdge(4, 0);
    graph.addEdge(4, 3);

    System.out.println("Breadth First Traversal : starting from 0 should be [0, 1, 4, 2, 3]");
    graph.BFS(0);
    System.out.println();
    System.out.println("Depth First Traversal : starting from 0 should be [0, 1, 2, 3, 4]");
    graph.DFS(0);
  }
}