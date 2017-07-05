'use strict';

function clearBit(num, i) {
  let mask = ~(1 << i);

  console.log(mask.toString(2));

  return num & mask;
}

function clearBitsMSBthroughI(num, i) {
  let mask = (1 << i) - 1;

  return num & mask;
}

console.log(clearBitsMSBthroughI(15, 3).toString(2));

console.log((4 >>> 0).toString(2));
process.exit(0);