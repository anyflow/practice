from typing import List

from radix_sort import sort


class Solution:
    def maximumGap(self, nums: List[int]) -> int:
        if len(nums) < 2:
            return 0

        sorted_nums = sort(nums)
        print(sorted_nums)

        ret = 0
        for i in range(1, len(sorted_nums)):
            diff = sorted_nums[i] - sorted_nums[i - 1]
            ret = max(ret, diff)

        return ret


if __name__ == "__main__":
    test_cases = [[3, 6, 9, 1]]
    solution = Solution()
    print([solution.maximumGap(tc) for tc in test_cases])
