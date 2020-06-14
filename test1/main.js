
const fs = require("fs");
const { execSync } = require('child_process');

const { vcd2msg } = require('./mod/vcd');
const { calcMapSizes } = require('./mod/map');

const vector = require('./mod/vector');

function runTestCase(testCase) {
    console.log('');
    console.log(`Test Case: ${testCase.name}`);
    console.log('='.repeat(testCase.name.length + 11));

    let results = {};

    vector.write(testCase);
    try {
        execSync('make clean');
        execSync('make -j4');
    } finally {
        vector.restore();
    }

    let map = calcMapSizes('test1.map', obj => (obj.file == "aes.o"));

    results.flash = map.flash;
    results.flashMax = map.flash;
    results.ram = map.ram;
    results.stackMax = 0;

    if (map.ramRemoved > 0 || map.flashRemoved > 0) throw Error("Test does not use all definitions from aes.o file!");

    try {
        fs.unlinkSync('aes_test.vcd');
    } catch (ex) { }
    execSync('run_avr test1.elf');

    let frequency = 20000000;
    let testName = '?';

    vcd2msg('aes_test.vcd', (time, name, data) => {
        if (name == 'frequency') {
            frequency = data;
        } else if (name == 'padding') {
            if (data) results.flashMax += 254;
        } else if (name == 'user') {
            results.ramUser = data;
        } else if (name == 'test') {
            console.log(data);
            testName = data;
            results[`${testName}.result`] = 1;
        } else if (name == 'time') {
            let stack = data.stackMax - data.stackMin;
            let ticks = data.time / 1000000000 * frequency;
            results[`${testName}.stack`] = stack;
            results[`${testName}.ticks`] = ticks;
            results.stackMax = Math.max(results.stackMax, stack);
        } else if (name.startsWith('result-')) {
            if (data == 0) {
                console.log(`    ERROR: ${testName}-${name}`);
                results[`${testName}.result`] = 0;
            }
        } else {
            console.log(time, name, data);
        }
    });

    testCases.results = results;

    return results;
}

let testCases = vector.readAll();
testCases = testCases.slice(0, 4);
for (tc of testCases) runTestCase(tc);

let html = `
<html><body><table border="1">
${
    testCases.map(testCase => `
    <tr>${
        Object.values(testCase.vector).map(val => `<td>${val}</td>`).join('')
    }</tr>
    `).join('')
}
</table></body></html>
`;

fs.writeFileSync('results.html', html);
