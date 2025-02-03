def sort(arr: list) -> list:
    shift = min(arr)
    for i, _ in enumerate(arr):
        arr[i] = arr[i] - shift

    # array의 각 index를 숫자로, 값은 갯수인 array 생성. max는 arr의 최대값.
    counts = [0] * (max(arr) + 1)
    for i in arr:
        counts[i] += 1

    start_index = 0
    for idx, count in enumerate(counts):
        counts[idx] = start_index
        start_index += count

    # 신규 array 생성
    new_arr = [0] * len(arr)
    for val in arr:
        new_arr[counts[val]] = val
        counts[val] += 1

    for i, val in enumerate(new_arr):
        arr[i] = val + shift

    return arr


lst = [-3, 100, 4, 2, 6891, 43, 4, 67, 2345, 547]

print(sort(lst))


# def sort(arr: list) -> list:

#     shift = 0
#     if (min_val := min(arr)) < 0:
#         shift = min_val
#         for i in range(len(arr)):
#             arr[i] -= shift

#     counts = max(arr)
#     counts = [0] * (counts + 1)

#     for count in arr:
#         counts[count] += 1

#     start_index = 0
#     for i, count in enumerate(counts):
#         counts[i] = start_index
#         start_index += count

#     new_list = [0] * len(arr)
#     for i in arr:
#         new_list[counts[i]] = i
#         counts[i] += 1

#     for i in range(len(arr)):
#         arr[i] = new_list[i]

#     if shift != 0:
#         for i in range(len(arr)):
#             arr[i] += shift

#     return arr


# lst = [-3, -100, 4, 2, 6891, 43, 4, 67, 2345, 547]

# print(sort(lst))
