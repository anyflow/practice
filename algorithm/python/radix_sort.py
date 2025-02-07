def digit_of(target: int, position: int) -> int:
    # complexities of time, space: O(1), O(1)
    return (target // (10**position)) % 10


def counting_sort(arr: list, position: int) -> list:
    # complexities of time, space: O(N), O(N)
    if len(arr) <= 0:
        return arr

    counts = [0] * 10  # Os(10)
    for item in arr:  # Ot(N)
        digit = digit_of(item, position)
        counts[digit] += 1

    index = [0] * 10  # Os(10)
    for idx in range(len(counts) - 1):  # Ot(N)
        index[idx + 1] = counts[idx] + index[idx]

    new_arr = [0] * len(arr)  # Os(N)
    for val in arr:  # Ot(N)
        digit = digit_of(val, position)
        new_arr[index[digit]] = val
        index[digit] += 1

    for i, val in enumerate(new_arr):  # Ot(N)
        arr[i] = val

    return arr


def digit_count_of(num):
    ret = 0
    while num > 0:
        ret += 1
        num //= 10

    return ret


def sort(arr):
    # complexities of time, space: Ot(digit_count) * Ot(N), O(N)
    if len(arr) <= 0:
        return arr

    shift = min(arr)
    arr = [v - shift for v in arr]  # Ot(N)

    digit_count = digit_count_of(max(arr))

    for i in range(digit_count):  # Ot(digit_count) * Ot(N)
        arr = counting_sort(arr, i)

    arr = [v + shift for v in arr]

    return arr


def test_sort():
    test_cases = [
        [170, 45, 75, 90, 802, 24, 2, 66],
        [-1, 1, 200, 3, 400, 5],
        [9, 8, 7, 6, 5, 4, 3, 2, 1],
        [1000, 1, 100, 10],
        [],  # 빈 배열 테스트
    ]

    for case in test_cases:
        original = case.copy()
        sorted_arr = sort(case)
        print(f"Original array: {original}")
        print(f"Sorted array:   {sorted_arr}")
        print("-" * 50)


# 테스트 실행
if __name__ == "__main__":
    test_sort()
    # print(radix_sort([170, 45, 75, 90, 802, 24, 2, 66]))


# def counting_sort(arr, exp):
#     """
#     특정 자릿수(exp)를 기준으로 counting sort를 수행하는 함수
#     exp는 1, 10, 100 등이 될 수 있음
#     """
#     n = len(arr)
#     output = [0] * n
#     count = [0] * 10  # 0-9까지의 숫자를 카운트

#     # 현재 자릿수를 기준으로 count 배열에 개수를 저장
#     for i in range(n):
#         index = arr[i] // exp
#         count[index % 10] += 1

#     # count 배열을 누적합으로 변환
#     for i in range(1, 10):
#         count[i] += count[i - 1]

#     # 뒤에서부터 순회하며 output 배열 구성
#     for i in range(n - 1, -1, -1):
#         index = arr[i] // exp
#         output[count[index % 10] - 1] = arr[i]
#         count[index % 10] -= 1

#     # 원본 배열에 정렬된 결과를 복사
#     for i in range(n):
#         arr[i] = output[i]


# def radix_sort(arr):
#     """
#     기수 정렬(Radix Sort) 구현
#     """
#     if not arr:
#         return arr

#     shift = min(arr)
#     for i in range(len(arr)):
#         arr[i] -= shift

#     # 배열에서 최대값 찾기
#     max_num = max(arr)

#     # 1의 자리부터 시작해서 최대값의 자릿수까지 정렬 수행
#     exp = 1
#     while max_num // exp > 0:
#         counting_sort(arr, exp)
#         exp *= 10

#     for i in range(len(arr)):
#         arr[i] += shift

#     return arr
