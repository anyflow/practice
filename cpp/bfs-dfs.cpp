#include <cassert>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <functional>

struct Node {
    int value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

void visit(const std::unique_ptr<Node>& node, std::vector<int>& output, const std::string& mode) {
    if (node == nullptr) {
        return;
    }

    if (mode == "level") {
        std::queue<const Node*> q;

        q.push(node.get());

        while (!q.empty()) {
            auto n = q.front();
            q.pop();

            output.push_back(n->value);
            if (n->left)
                q.push(n->left.get());
            if (n->right)
                q.push(n->right.get());
        }

        return;
    }

    if (mode == "pre") {
        output.push_back(node->value);
    }
    visit(node->left, output, mode);
    if (mode == "in") {
        output.push_back(node->value);
    }
    visit(node->right, output, mode);
    if (mode == "post") {
        output.push_back(node->value);
    }
}

void p1() {
    auto root = std::make_unique<Node>(
        1, std::make_unique<Node>(2, std::make_unique<Node>(4), std::make_unique<Node>(5)), std::make_unique<Node>(3));

    std::vector<int> output;
    visit(root, output, "pre");
    assert((output == std::vector<int>{1, 2, 4, 5, 3}));

    output.clear();
    visit(root, output, "in");
    assert((output == std::vector<int>{4, 2, 5, 1, 3}));

    output.clear();
    visit(root, output, "post");
    assert((output == std::vector<int>{4, 5, 2, 3, 1}));

    output.clear();
    visit(root, output, "level");
    assert((output == std::vector<int>{1, 2, 3, 4, 5}));
}

using Graph = std::vector<std::vector<int>>;

std::vector<int> bfsOrder(const Graph& graph, int start) {
    if(start >= static_cast<int>(graph.size())) {
        return {};
    }

    std::vector<bool> visited(graph.size(), false);
    std::queue<int> queue;
    visited[start] = true;
    queue.push(start);

    std::vector<int> result;
    while (!queue.empty()) {
        auto curr = queue.front();
        queue.pop();
        result.push_back(curr);

        for(const int next: graph[curr]) {
            if(visited[next]) {
                continue;
            }
            visited[next] = true;
            queue.push(next);
        }
    }

    return result;
}

std::vector<int> dfsOrderIterative(const Graph& graph, int start) {
    if(start >= static_cast<int>(graph.size())) {
        return {};
    }

    std::vector<bool> visited(graph.size(), false);
    std::stack<int> stack;
    visited[start] = true;

    stack.push(start);

    std::vector<int> result;
    while (!stack.empty()) {
        auto curr = stack.top();
        result.push_back(curr);
        stack.pop();

        for(const int next: graph[curr]) {
            if(visited[next]) {
                continue;
            }
            visited[next] = true;
            stack.push(next);
        }
    }

    return result;
}

std::vector<int> dfsOrderRecursive(const Graph& graph, int start) {
    if(start >= static_cast<int>(graph.size())) {
        return {};
    }

    std::vector<bool> visited(graph.size(), false);
    std::vector<int> result;

    std::function<void(int)> dfs = [&result, &visited, &graph, &dfs](int node) {
        result.push_back(node);

        for(const int next: graph[node]) {
            if(visited[next]) {
                continue;
            }
            visited[next] = true;
            dfs(next);
        }
    };

    visited[start] = true;
    dfs(start);
    return result;
}

std::vector<int> shortestDistances(const Graph& graph, int start) {
    if(start < 0 || start >= static_cast<int>(graph.size())) {
        return {};
    }

    std::queue<std::pair<int, int>> q;
    std::vector<int> distance(graph.size(), -1);

    q.push(std::make_pair(start, -1));

    while(!q.empty()) {
        auto [curr, prev_dist] = q.front();
        q.pop();
        distance[curr] = prev_dist + 1;

        for (auto next : graph[curr]) {
            if(distance[next] != -1) {
                continue;
            }
            q.push(std::make_pair(next, distance[curr]));
        }
    }

    return distance;
}

void p2() {
    std::vector<std::vector<int>> g1 = {{1, 2}, {3}, {3}, {}, {}};
    std::vector<std::vector<int>> g2 = {{1}, {2}, {0, 3}, {}};
    std::vector<std::vector<int>> g3 = {{1}, {}, {3}, {}};
    std::vector<std::vector<int>> g4 = {{}};
    std::vector<std::vector<int>> g5 = {};
    int s1 = 0, s2 = 0, s3 = 2, s4 = 0, s5 = 0;

    assert((bfsOrder(g1, s1) == std::vector<int>{0, 1, 2, 3}));
    assert((bfsOrder(g2, s2) == std::vector<int>{0, 1, 2, 3}));
    assert((bfsOrder(g3, s3) == std::vector<int>{2, 3}));
    assert((bfsOrder(g4, s4) == std::vector<int>{0}));
    assert((bfsOrder(g5, s5) == std::vector<int>{}));

    assert((dfsOrderIterative(g1, s1) == std::vector<int>{0, 2, 3, 1}));
    assert((dfsOrderIterative(g2, s2) == std::vector<int>{0, 1, 2, 3}));
    assert((dfsOrderIterative(g3, s3) == std::vector<int>{2, 3}));
    assert((dfsOrderIterative(g4, s4) == std::vector<int>{0}));
    assert((dfsOrderIterative(g5, s5) == std::vector<int>{}));

    assert((dfsOrderRecursive(g1, s1) == std::vector<int>{0, 1, 3, 2}));
    assert((dfsOrderRecursive(g2, s2) == std::vector<int>{0, 1, 2, 3}));
    assert((dfsOrderRecursive(g3, s3) == std::vector<int>{2, 3}));
    assert((dfsOrderRecursive(g4, s4) == std::vector<int>{0}));
    assert((dfsOrderRecursive(g5, s5) == std::vector<int>{}));
}

void p3() {
    std::vector<std::vector<int>> g1 = {{1, 2}, {3}, {3}, {}, {}};
    std::vector<std::vector<int>> g2 = {{1}, {2}, {0, 3}, {}};
    std::vector<std::vector<int>> g3 = {{1}, {}, {3}, {}};
    std::vector<std::vector<int>> g4 = {{}};
    std::vector<std::vector<int>> g5 = {};
    int s1 = 0, s2 = 0, s3 = 2, s4 = 0, s5 = 0;

    assert((shortestDistances(g1, s1) == std::vector<int>{0, 1, 1, 2, -1}));
    assert((shortestDistances(g2, s2) == std::vector<int>{0, 1, 2, 3}));
    assert((shortestDistances(g3, s3) == std::vector<int>{-1, -1, 0, 1}));
    assert((shortestDistances(g4, s4) == std::vector<int>{0}));
    assert((shortestDistances(g5, s5) == std::vector<int>{}));
}

int main() {
    p1();
    p2();
    p3();
    return 0;
}