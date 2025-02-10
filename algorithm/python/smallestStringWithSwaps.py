from collections import defaultdict
from typing import List


class DisjointSet:
    def __init__(self, n: int):
        self.root = [i for i in range(n)]
        self.rank = [1] * n

    def find(self, x: int):
        if x == self.root[x]:
            return x

        self.root[x] = self.find(self.root[x])

        return self.root[x]

    def union(self, x: int, y: int) -> bool:
        root_x = self.find(x)
        root_y = self.find(y)

        if root_x == root_y:
            return False

        if self.rank[root_x] > self.rank[root_y]:
            self.root[root_y] = root_x
        elif self.rank[root_y] > self.rank[root_x]:
            self.root[root_x] = root_y
        else:
            self.root[root_y] = root_x
            self.rank[root_x] += 1

        return True


class Solution:
    def smallestStringWithSwaps(self, s: str, pairs: List[List[int]]) -> str:
        """
        1. unique root 별 string index 찾기
        2. 각 unique root 별로 sorting
        3. merge
        """
        count = len(s)
        ds = DisjointSet(count)

        for x, y in pairs:
            ds.union(x, y)

        values = defaultdict(list)
        indexes = defaultdict(list)
        for i in range(count):
            root = ds.find(i)
            values[root].append(s[i])
            indexes[root].append(i)

        ret = [None] * count
        for root in ds.root:
            sorted_idx = indexes[root]  # sort is not need since it's already sorted
            sorted_s = sorted(values[root])

            for i in range(len(sorted_s)):
                ret[sorted_idx[i]] = sorted_s[i]

        return "".join(ret)


if __name__ == "__main__":
    tcs = [("dcab", [[0, 3], [1, 2]]), ("dcab", [[0, 3], [1, 2], [0, 2]])]
    sol = Solution()

    for s, pairs in tcs:
        print(sol.smallestStringWithSwaps(s, pairs))
