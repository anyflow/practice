
class Heap<T extends Comparable<T>> {
    class Level {
        int level;

        Level(int level) {
            this.level = level;
        }
    }

    class Node {
        T data;
        Node parent = null;
        Node left = null;
        Node right = null;

        Node(T data, Node parent) {
            this.data = data;
            this.parent = parent;
        }

        void insert(Node target) throws Exception {
            if (left != null && right != null) {
                throw new Exception("Unable to insert");
            }

            if (left == null) {
                left = target;
            } else {
                right = target;
            }

            Node toSwap = target;
            while (toSwap != null) {
                toSwap = toSwap.swapParent();
            }
        }

        boolean hasRoom() {
            return right == null;
        }

        Node swapParent() {
            if (parent == null || parent.data.compareTo(data) < 0) {
                return null;
            }

            T temp = parent.data;
            parent.data = data;
            data = temp;
            return parent;
        }

        @Override
        public String toString() {
            return data.toString();
        }
    }

    Node top = null;
    Node roomToInsert = null;
    Node last = null;

    void insert(T data) throws Exception {
        last = new Node(data, roomToInsert);

        if (roomToInsert == null) {
            roomToInsert = last;
            top = roomToInsert;
            return;
        }

        roomToInsert.insert(last);

        if (roomToInsert.hasRoom()) {
            return;
        }

        roomToInsert = findRoom(top, new Level(0));
    }

    Node findLast(Node head, Level level) {
        if (head == null) {
            return null;
        }

        level.level++;

        if (head.left == null && head.right == null) {
            return head;
        }

        Level leftLevel = new Level(level.level);
        Node left = findLast(head.left, leftLevel);

        Level rightLevel = new Level(level.level);
        Node right = findLast(head.right, rightLevel);

        if (right == null || leftLevel.level > rightLevel.level) {
            level.level = leftLevel.level;
            return left;
        } else {
            level.level = rightLevel.level;
            return right;
        }
    }

    Node findRoom(Node head, Level level) {
        if (head == null) {
            return null;
        }

        level.level++;

        if (head.hasRoom()) {
            return head;
        }

        Level leftLevel = new Level(level.level);
        Node left = findRoom(head.left, leftLevel);

        Level rightLevel = new Level(level.level);
        Node right = findRoom(head.right, rightLevel);

        if (leftLevel.level <= rightLevel.level) {
            level.level = leftLevel.level;
            return left;
        } else {
            level.level = rightLevel.level;
            return right;
        }
    }

    Node swapChild(Node node) {
        if (node.left == null) {
            return null;
        }

        Node target = null;
        if (node.right == null || node.left.data.compareTo(node.right.data) < 0) {
            target = node.data.compareTo(node.left.data) < 0 ? null : node.left;
        } else {
            target = node.data.compareTo(node.right.data) < 0 ? null : node.right;
        }

        if (target != null) {
            T temp = node.data;
            node.data = target.data;
            target.data = temp;
        }
        return target;
    }

    T pop() {
        T ret = peek();
        if (ret == null) {
            return null;
        }

        if (last == top) {
            top = last = roomToInsert = null;
            return ret;
        }

        top.data = last.data;

        if (last.parent.left == last) {
            last.parent.left = null;
        }
        if (last.parent.right == last) {
            last.parent.right = null;
        }
        roomToInsert = last.parent;
        last = findLast(top, new Level(0));

        Node toSwap = top;
        while (toSwap != null) {
            toSwap = swapChild(toSwap);
        }

        return ret;
    }

    T peek() {
        return top == null ? null : top.data;
    }

    static class Item implements Comparable<Item> {
        Integer value;

        Item(Integer value) {
            this.value = value;
        }

        @Override
        public int compareTo(Item other) {
            return other.value - value;
        }

        @Override
        public String toString() {
            return value.toString();
        }
    }

    public static void main(String[] args) {
        Heap<Item> heap = new Heap<>();

        try {
            heap.insert(new Item(6));
            heap.insert(new Item(1));
            heap.insert(new Item(3));
            heap.insert(new Item(2));
            heap.insert(new Item(5));
            heap.insert(new Item(4));
            heap.insert(new Item(7));

            Item inserted = heap.pop();
            while (inserted != null) {
                System.out.println(inserted.value);
                inserted = heap.pop();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}