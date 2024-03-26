#! /usr/bin/env python3
# vim:fenc=utf-8
#
# Copyright © 2024 Letian Lin <yingziyu-Lin@outlook.com>
#
# Distributed under terms of the GNU AGPLv3+ license.
import os


class Solution(object):
    def twoSum(self, nums, target):
        """
        :type nums: List[int]
        :type target: int
        :rtype: List[int]
        """
        newNums = []
        for idx, i in enumerate(nums):
            newNums.append((i, idx))
        newNums.sort()
        n = len(nums)
        for idx, i in enumerate(newNums):
            l = idx + 1
            r = n - 1
            while l < r:
                mid = (l + r) // 2
                if newNums[mid][0] > target - newNums[idx][0]:
                    r = mid
                elif newNums[mid][0] < target - newNums[idx][0]:
                    l = mid + 1
                else:
                    l = mid
                    break
            if newNums[l][0] + newNums[idx][0] == target:
                return [newNums[l][1], newNums[idx][1]]


sol = Solution()
print(sol.twoSum([-1, -2, -3, -4, -5], -8))
