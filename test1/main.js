
const fs = require("fs");
const { execSync } = require('child_process');

const { vcd2msg } = require('./mod/vcd');
const { calcMapSizes } = require('./mod/map');

function readVectors() {
    let lines = fs.readFileSync('vectors.txt', 'UTF-8')
        .split(/\s*\n\s*/)
        .filter(x => x.trim() != '');
    let names = [];
    lines = lines.filter(line => {
        let m = line.match(/^((?:\s*\|\s*)*)([A-Za-z0-9_]*)$/);
        if (!m) return true;
        if (m[2] == '') return false;
        let index = m[1].replace(/[^\|]/g, '').length;
        names[index] = m[2];
        return false;
    });
    let vectors = [];
    for (let line of lines) {
        let items = line.split(/\s+/);
        let vector = {};
        let name = [];
        for (let i = 0; i < items.length; i++) {
            vector[names[i]] = items[i].replace('-', '_undefined_');
            if (items[i] != '-') name.push(`${names[i].replace(/^AES_/, '')}=${items[i]}`);
        }
        vectors.push({
            name: name.join('; '),
            vector: vector
        });
    }
    return vectors;
}

function writeVector(testCase) {
    let h = fs.readFileSync('../aes.h', 'UTF-8');
    fs.renameSync('../aes.h', '../_aes_copy.h_');
    for (let name in testCase.vector) {
        let value = testCase.vector[name];
        let h2 = h.replace(new RegExp(`(\n#define\\s+${name}\\s+)(.+?)(\r?\n)`), `$1((((${value}))))$3`);
        if (h2 == h) throw Error(`Definition '${name}'`);
        h = h2;
    }
    fs.writeFileSync('../aes.h', h);
}

function restoreVector() {
    fs.unlinkSync('../aes.h');
    fs.renameSync('../_aes_copy.h_', '../aes.h');
}

function runTestCase(testCase) {
    console.log('=================================================');
    console.log(`Test Case: ${testCase.name}`);
    console.log('=================================================');

    let results = {};

    writeVector(testCase);
    try {
        execSync('make clean');
        execSync('make -j4');
    } finally {
        restoreVector();
    }

    let map = calcMapSizes('test1.map', obj => (obj.file == "aes.o"));

    results.flash = map.flash;
    results.flashMax = map.flash;
    results.ram = map.ram;
    results.stackMax = 0;

    //if (map.ramRemoved > 0 || map.flashRemoved > 0) throw Error("Test does not use all definitions from aes.o file!");

    try {
        fs.unlinkSync('aes_test.vcd');
    } catch (ex) { }
    execSync('run_avr test1.elf');

    let frequency = 20000000;
    let testName = '?';
    let testBytes = 0;

    vcd2msg('aes_test.vcd', (time, name, data) => {
        if (name == 'frequency') {
            frequency = data;
        } else if (name == 'padding') {
            if (data) results.flashMax += 254;
        } else if (name == 'test') {
            console.log(data);
            testName = data;
            testBytes = 0;
            results[`${testName}-result`] = 1;
        } else if (name == 'time') {
            let stack = data.stackMax - data.stackMin;
            let ticks = data.time / 1000000000 * frequency;
            results[`${testName}-stack`] = stack;
            results[`${testName}-ticks`] = ticks;
            results.stackMax = Math.max(results.stackMax, stack);
            if (testBytes > 0) results[`${testName}-speed`] = testBytes / (data.time / 1000000000);
        } else if (name == 'bytes') {
            testBytes = data;
        } else if (name.startsWith('result-')) {
            if (data == 0) {
                console.log(`    ERROR: ${testName}-${name}`);
                results[`${testName}-result`] = 0;
            }
        } else {
            console.log(time, name, data);
        }
    });

    console.log(results);
}

let testCases = readVectors();
for (tc of testCases) runTestCase(tc);
