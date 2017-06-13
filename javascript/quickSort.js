'use strict';

let input = [3, 1, 5, 9, 0, 10, 5, 32, -1, -20, 239, 583, 6, 3, 54, 53, 38];

function sort(arr, left, right) {
    if (arr.length <= 1) {
        return arr;
    }

    let buffer = null;
    let pivotIndex = parseInt((left + right) / 2);
    let pivot = arr[pivotIndex];
    swap(arr, pivotIndex, right - 1);

    let j = right - 2;
    let i = left;
    while (i <= j) {
        if (arr[i] > pivot) {
            if (arr[j] > pivot) {
                --j;
            } else {
                swap(arr, i, j);
            }
        } else {
            if (arr[j] > pivot) {
                --j;
            } else {
                swap(arr, i, j);
            }

            ++i;
        }
    }

    swap(arr, i, right - 1);

    if (i > left) {
        sort(arr, left, i);
    }

    if (j < right - 2) {
        sort(arr, i, right);
    }

    return arr;
}

function swap(arr, a, b) {
    let buffer = null;

    buffer = arr[a];
    arr[a] = arr[b];
    arr[b] = buffer;
}


console.log(`before(size:${input.length}) : ${input}`);

sort(input, 0, input.length);

console.log(`after(size:${input.length})  : ${input}`);
process.exit(0);