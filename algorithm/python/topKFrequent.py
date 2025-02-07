from typing import List


class Solution:
    def topKFrequent(self, nums: List[int], k: int) -> List[int]:
        # count map 구성: Ot(N), Os(N)
        # map의 key를 value 기준으로 sort: Ot(NlogN), Os(N)
        # left most k개의 element 반환 Ot(N), O(1)

        # Ot(N + N + NlogN => NlogN), Os(N + N + 1 => N)

        count_map = {}
        for key in nums:
            if key not in count_map:
                count_map[key] = 1
            else:
                count_map[key] += 1
        print(count_map)
        sorted_items = sorted(
            [(key, v) for key, v in count_map.items()], key=lambda x: x[1], reverse=True
        )

        print(sorted_items)
        return [sorted_items[i][0] for i in range(k)]


if __name__ == "__main__":
    sol = Solution()

    print(sol.topKFrequent([1, 1, 1, 2, 3], 2))
