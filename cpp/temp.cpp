#include <iostream>
using namespace std;

class Node {
public:
    int data;
    Node* next;

    Node(int data) {
        this->data = data;
        next = nullptr;
    }

    Node(int data, Node* next) {
        this->data = data;
        this->next = next;
    }
};

int main() {
    Node first = Node(1);
    Node second = Node(1, &first);
    cout << "hello" << endl;
}