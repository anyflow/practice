'use strict';

let input = [3, 1, 5, 9, 0, 10, 5, 32, -1, -20, 239, 583, 6, 3, 54, 53, 38];

function sort(arr) {
  let temp, j;
  for(let i = 1; i < arr.length; ++i) {
    temp = arr[i];

    j = i - 1;

    while(j >= 0 && arr[j] > temp) {
      arr[j + 1] = arr[j];
      --j;
    }

    arr[j + 1] = temp;
  }
}

console.log(`before(size:${input.length}) : ${input}`);

sort(input);

console.log(`after(size:${input.length})  : ${input}`);
process.exit(0);