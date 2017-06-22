class Heap<T> {
    class Node {
        T data;
        Node left = null;
        Node right = null;

        Node(T data) {
            this.data = data;
        }

        void insert(T data) throws Exception {
            if (left != null && right != null) {
                throw Exception("Unable to insert");
            }

            T target = new Node(data);
            if (left == null) {
                left = target;
            } else {
                right = target;
            }
        }

        boolean hasRoom() {
            return right == null;
        }
    }

    Node top = null;
    Node roomToInsert = null;

    void insert(T data) {
        if (roomToInsert == null) {
            roomToInsert = data;
            top = roomToInsert;
            return;
        }

        roomToInsert.insert(data);

        if (roomToInsert.hasRoom()) {
            return;
        }

        roomToInsert = findRoom();
    }

    Node findRoom() {

    }

    void delete() {

    }

    public static void main(String[] args) {

    }
}