#include <functional>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

class Solution {
  public:
    tuple<ListNode*, ListNode*> tailWithReversing(ListNode* head) {
        if (head->next == nullptr) {
            return make_tuple(head, head);
        }

        auto pair = tailWithReversing(head->next);
        if (get<1>(pair) != nullptr) {
            get<1>(pair)->next = head;
            return make_tuple(get<0>(pair), head);
        } else {
            return make_tuple(head, head);
        }
    }

    ListNode* reverseList(ListNode* head) {
        if (head == nullptr) {
            return nullptr;
        }
        auto ret = tailWithReversing(head);
        head->next = nullptr;

        return get<0>(ret);
    }

    double myPow(double x, int n) {
        if (x == 0 || x == 1) {
            return x;
        }
        if (n == 0) {
            return 1;
        }

        if (n < 0) {
            return (1 / x) * (1 / myPow(x, -1 * (n + 1)));
        }

        map<int, double> table = {{1, x}};

        double val = x, indexVal = 1;
        for (; indexVal < n;) {
            auto maxPtr = table.upper_bound(n - indexVal);
            maxPtr--;

            indexVal += maxPtr->first;
            val *= maxPtr->second;

            table[indexVal] = val;
        }

        return val;
    }
};

int main() {
    // auto head = new ListNode(1, new ListNode(2, new ListNode(3, nullptr)));
    // head = Solution().reverseList(head);

    // while (head != nullptr) {
    //     cout << head->val << "|";
    //     head = head->next;
    // }

    int theNumber = -2147483648;
    printf("%zu %d %zu\n", sizeof(-2147483649), theNumber, sizeof(theNumber));

    int a = pow(2, 31);
    cout << "a : " << a << "| a - 1 : " << a - 1 << endl;

    // Solution().myPow(2.0, -2147483648);

    cout << (~23 + 1) + (~12 + 1) << endl;
    return 0;
}