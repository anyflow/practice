from collections import defaultdict
from typing import List


class Solution:
    def calcEquation(
        self, equations: List[List[str]], values: List[float], queries: List[List[str]]
    ) -> List[float]:
        """
        1. create map
        # key: origin equation and inverted equation
        # value: their values
        """
        ex_equations = defaultdict(dict)

        for i in range(len(equations)):
            x = equations[i][0]
            y = equations[i][1]

            ex_equations[x][y] = values[i]
            ex_equations[y][x] = 1 / values[i]

        def solve(x: str, y: str) -> float:
            if x not in ex_equations:
                return float(-1)

            if x == y:
                return float(1)

            def accumulate(initial, _1, stop, visited=set()):
                visited.add(_1)

                try:
                    _2 = next(
                        i for i in ex_equations[_1] if i not in visited
                    )  # TODO ["x2", "x4"] 에서 오류. x2 -> x3 -> x4로 가야하지만, x2 -> x1으로 가기 때문에.
                    # TODO 결국 path search해야.

                except StopIteration as e:
                    raise e

                if not _2:
                    return -1

                visited.add(_2)

                initial *= ex_equations[_1][_2]
                if _2 == stop:
                    return initial
                else:
                    return accumulate(initial, _2, stop, visited)

            return accumulate(1, x, y)

        return [solve(x, y) for x, y in queries]


if __name__ == "__main__":
    sol = Solution()

    tcs = [
        # (
        #     [["a", "b"], ["b", "c"]],
        #     [2.0, 3.0],
        #     [["a", "c"], ["b", "a"], ["a", "e"], ["a", "a"], ["x", "x"]],
        # ),
        # (
        #     [["a", "b"], ["b", "c"], ["bc", "cd"]],
        #     [1.5, 2.5, 5.0],
        #     [["a", "c"], ["c", "b"], ["bc", "cd"], ["cd", "bc"]],
        # ),
        # ([["a", "b"]], [0.5], [["a", "b"], ["b", "a"], ["a", "c"], ["x", "y"]]),
        (
            [["x1", "x2"], ["x2", "x3"], ["x3", "x4"], ["x4", "x5"]],
            [3.0, 4.0, 5.0, 6.0],
            [
                # ["x1", "x5"],
                # ["x5", "x2"],
                ["x2", "x4"],
                # ["x2", "x2"],
                # ["x2", "x9"],
                # ["x9", "x9"],
            ],
        ),
    ]

    [print(sol.calcEquation(_1, _2, _3)) for _1, _2, _3 in tcs]


# class DisjointSet:
#     def __init__(self):
#         self.root = defaultdict(str)
#         self.rank = defaultdict(int)

#     def add(self, xs: List[str]) -> None:
#         for x in xs:
#             self.root[x] = x
#             self.rank[x] = 1

#     def find(self, x: str) -> str:
#         if x not in self.root:
#             return None

#         if self.root[x] == x:
#             return x

#         self.root[x] = self.find(self.root[x])
#         return self.root[x]

#     def union(self, x: str, y: str) -> bool:
#         if x not in self.root or y not in self.root:
#             return False

#         root_x = self.find(x)
#         root_y = self.find(y)

#         if root_x == root_y:
#             return False

#         if self.rank[root_x] > self.rank[root_y]:
#             self.root[root_y] = root_x
#         elif self.rank[root_x] < self.rank[root_y]:
#             self.root[root_x] = root_y
#         else:
#             self.root[root_y] = root_x
#             self.rank[root_x] += 1

#         return True

#     def connected(self, x: str, y: str) -> bool:
#         if x not in self.root or y not in self.root:
#             return False

#         return self.find(x) == self.find(y)
