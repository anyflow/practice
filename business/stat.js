'use strict'

const path = require('path');
const xlsx = require('xlsx');
const fs = require('fs');
const mathjs = require('mathjs');

const defaultDir = __dirname + '/data';

const weightProductCountInverse = 1;
const weightClickCount = 1;
const weightTotalSearchCount = 1;
const weightTotalCompeteRateInverse = 1;
const weightTotalClickRate = 1;

const all = [];

fs.readdirSync(defaultDir).forEach(category1 => {
  if (!fs.lstatSync(`${defaultDir}/${category1}`).isDirectory()) {
    return;
  }

  fs.readdirSync(`${defaultDir}/${category1}`).forEach(category2 => {
    if (!fs.lstatSync(`${defaultDir}/${category1}/${category2}`).isDirectory()) {
      return;
    }

    all.push(generateCategory2(defaultDir, category1, category2));
  })
});

const meanOfTotalSearchCount = mathjs.mean(all.map(item => item.totalSearchCount));
const meanOfProductCountInverse = mathjs.mean(all.map(item => 1 / item.productCount));
const meanOfClickCount = mathjs.mean(all.map(item => item.clickCount));
const meanOfTotalCompeteRateInverse = mathjs.mean(all.map(item => 1 / item.totalCompeteRate));
const meanOfTotalClickRate = mathjs.mean(all.map(item => item.totalClickRate));

const stdevOfTotalSearchCount = mathjs.std(all.map(item => item.totalSearchCount));
const stdevOfProductCountInverse = mathjs.std(all.map(item => 1 / item.productCount));
const stdevOfClickCount = mathjs.std(all.map(item => item.clickCount));
const stdevOfTotalCompeteRateInverse = mathjs.std(all.map(item => 1 / item.totalCompeteRate));
const stdevOfTotalClickRate = mathjs.std(all.map(item => item.totalClickRate));

all.forEach(item => {
  item.rateTotalSearchCount = (item.totalSearchCount - meanOfTotalSearchCount) / stdevOfTotalSearchCount;
  item.rateProductCountInverse = (1 / item.productCount - meanOfProductCountInverse) / stdevOfProductCountInverse;
  item.rateClickCount = (item.clickCount - meanOfClickCount) / stdevOfClickCount;
  item.rateTotalCompeteRateInverse = (1 / item.totalCompeteRate - meanOfTotalCompeteRateInverse) / stdevOfTotalCompeteRateInverse;
  item.rateTotalClickRate = (item.totalClickRate - meanOfTotalClickRate) / stdevOfTotalClickRate;

  item.metric1 = item.rateTotalSearchCount * weightTotalSearchCount +
    item.rateProductCountInverse * weightProductCountInverse +
    item.rateClickCount * weightClickCount;

  item.metric2 = item.rateTotalSearchCount * weightTotalSearchCount +
    item.rateTotalCompeteRateInverse * weightTotalCompeteRateInverse +
    item.rateTotalClickRate * weightTotalClickRate;
});
// console.log(JSON.stringify(all, null, 2));

const newWorkbook = xlsx.utils.book_new();
const newSheet = xlsx.utils.json_to_sheet(all);

xlsx.utils.book_append_sheet(newWorkbook, newSheet, 'stats');
xlsx.writeFileSync(newWorkbook, `${defaultDir}/stats.xlsx`)


function generateCategory2(defaultDir, category1, category2) {
  const statDir = `${defaultDir}/${category1}/${category2}`;
  const fileNames = [];

  fs.readdirSync(statDir).forEach(f => {
    if (f.endsWith('xls')) {
      fileNames.push(f);
    }
  });

  fileNames.sort((a, b) => {
    return parseInt(a.split('.')[0]) - parseInt(b.split('.')[0]);
  });

  let results = {
    items: [],
    category1: category1,
    category2: category2,
    keywordCount: 0,
    searchCountInPC: 0,
    searchCountInMobile: 0,
    productCount: 0,
    clickCount: 0
  };

  fileNames.forEach(fileName => {
    results.items.push(generateItem(fileName, statDir));
  });

  results.items.forEach(item => {
    results['keywordCount'] += item['keywordCount'];
    results['searchCountInPC'] += item['searchCountInPC'];
    results['searchCountInMobile'] += item['searchCountInMobile'];
    results['productCount'] += item['productCount'];
    results['clickCount'] += item['clickCount'];
  });

  const totalSearchCount = results.searchCountInPC + results.searchCountInMobile;

  results = {
    ...results,
    totalSearchCount: totalSearchCount,
    totalCompeteRate: totalSearchCount / results.productCount,
    totalClickRate: results.clickCount / totalSearchCount,
  };

  fs.writeFileSync(statDir + '/results.json', JSON.stringify(results, null, 2));

  delete results.items;

  console.log(JSON.stringify(results, null, 2));

  return results;
}

function generateItem(fileName, basePath) {
  const workbook = xlsx.readFile(path.resolve(basePath, fileName));

  const firstSheetName = workbook.SheetNames[0];
  const sheet = workbook.Sheets[firstSheetName];

  const jsonized = xlsx.utils.sheet_to_json(sheet);

  let result = {
    fileName: fileName,
    keywordCount: jsonized.length,
    keywords: [],
    searchCountInPC: 0,
    searchCountInMobile: 0,
    productCount: 0,
    clickCount: 0
  };

  jsonized.forEach(e => {
    // result.keywords.push(e['키워드']);
    result['searchCountInPC'] += e['PC 검색'];
    result['searchCountInMobile'] += e['모바일 검색'];
    result['productCount'] += e['상품수'];
    result['clickCount'] += e['PC 검색'] * e['PC클릭률'] + e['모바일 검색'] * e['모바일클릭률'];
  });

  const totalSearchCount = result.searchCountInPC + result.searchCountInMobile;

  result = {
    ...result,
    totalSearchCount: totalSearchCount,
    totalCompeteRate: totalSearchCount / result.productCount,
    totalClickRate: result.clickCount / totalSearchCount
  }

  return result;
}