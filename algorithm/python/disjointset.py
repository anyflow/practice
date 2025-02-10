class DisjointSet:
    def __init__(self, n: int):
        self.parent = [i for i in range(n)]
        self.rank = [1] * n

    def find(self, x: int) -> int:
        if x == self.parent[x]:
            return x
        self.parent[x] = self.find(self.parent[x])
        return self.parent[x]

    def union(self, x: int, y: int) -> None:
        rootX = self.find(x)
        rootY = self.find(y)
        if rootX == rootY:
            return rootX

        if self.rank[rootX] > self.rank[rootY]:
            self.parent[rootY] = rootX
        elif self.rank[rootX] < self.rank[rootY]:
            self.parent[rootX] = rootY
        else:
            self.parent[rootY] = rootX
            self.rank[rootX] += 1

    def connected(self, x: int, y: int):
        return self.find(x) == self.find(y)


if __name__ == "__main__":
    ds = DisjointSet(4)

    print(ds.union(0, 1), ds.parent, ds.rank)
    print(ds.union(2, 3), ds.parent, ds.rank)
    print(ds.union(0, 2), ds.parent, ds.rank)
