import java.util.*;

class Heap2<T extends Comparable<T>> {
    List<T> items = new ArrayList<>();

    class Level {
        int level;

        Level(int level) {
            this.level = level;
        }
    }

    public void insert(T data) {
        items.add(data);

        if (items.size() == 0) {
            return;
        }

        swapUp(items.size() - 1);
    }

    boolean swapUp(int childIndex) {
        Integer parentIndex = parent(childIndex);
        if(parentIndex == null) { return false; }

        T parentItem = items.get(parentIndex);
        T childItem = items.get(childIndex);

        if (parentItem.compareTo(childItem) >= 0) {
            items.set(parentIndex, childItem);
            items.set(childIndex, parentItem);

            return swapUp(parentIndex);
        } else {
            return false;
        }
    }

    boolean swapDown(int parentIndex) {
        int leftIndex = parentIndex * 2 + 1;
        int rightIndex = parentIndex * 2 + 2;

        if (parentIndex < 0 || leftIndex >= items.size()) {
            return false;
        }

        int targetIndex;
        if(rightIndex >= items.size()) {
            targetIndex = leftIndex;
        } else {
            T left = items.get(leftIndex);
            T right = items.get(rightIndex);

            targetIndex = left.compareTo(right) <= 0 ? leftIndex : rightIndex;
        }

        T parent = items.get(parentIndex);
        T target = items.get(targetIndex);

        if (parent.compareTo(target) <= 0) {
            return false;
        } else {
            items.set(parentIndex, target);
            items.set(targetIndex, parent);

            return swapDown(targetIndex);
        }


    }

    public T pop() {
        if (items.size() <= 0) {
            return null;
        }

        T ret = items.get(0);

        items.set(0, items.get(items.size() - 1));
        items.remove(items.size() - 1);

        swapDown(0);

        return ret;
    }

    public T peek() {
        return items.get(0);
    }

    Integer parent(int childIndex) {
        if (childIndex <= 0) {
            return null;
        }

        return (int)Math.floor((childIndex - 1) / 2);
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
        Heap2<Item> heap = new Heap2<>();

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
    }
}