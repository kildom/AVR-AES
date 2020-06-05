const fs = require("fs");


let vcd = fs.readFileSync('C:/work/AVR-AES/simavr/simavr/obj-i686-w64-mingw32/aes_test.vcd', 'UTF-8');

function vcd2reg(vcd, callback) {
    let start = (/^#/m).exec(vcd).index;
    let header = vcd.substr(0, start);
    let data = vcd.substr(start);
    let [, step, unit] = header.match(/\$timescale\s+([0-9]+)(.+?)\s+\$end/);
    switch (unit) {
        case 's': step *= 1000;
        case 'ms': step *= 1000;
        case 'us': step *= 1000;
        case 'ns': break;
        default: throw Error(`Unknown unit ${unit}`);
    }
    let nameOf = {};
    header.replace(/\$var\s+wire\s+8\s+(\S+)\s+(.+?)\s+\$end/g, (__, char, name) => {
        nameOf[char] = name;
    });
    let now = 0;
    let out = data.replace(/^(?:#([0-9]+))|(?:b([0-1]{8}) (.))$/gm, (__, time, bits, char) => {
        if (typeof (time) != 'undefined') {
            now = parseFloat(time) * step;
        } else {
            //regs.push({time: now, name: nameOf[char], value: parseInt(bits, 2)});
            callback(now, nameOf[char], parseInt(bits, 2));
        }
        return '';
    });
}

let stack = 0;
let stackSPL = 0;
let stackSPH = 0;
let stackLast = '';
let stackTime = 0;
let maxStackDelay = 0;

let data = [];

function updateStack(time, value, expected, current)
{
    if (stackLast == expected) {
        stack = stackSPL | stackSPH << 8;
        maxStackDelay = Math.max(maxStackDelay, time - stackTime); // TODO: check if time not too long
        console.log(time, maxStackDelay, stack);
        stackLast = '';
    } else if (stackLast == current) {
        throw Error('Unexpected sequence of SPL/SPH. Unable to correctly interpret stack size.');
    } else {
        stackLast = current;
    }
    stackValue = value;
    stackTime = time;
}

function executeCommand(id, data)
{
    console.log('CMD', id, data);
}

vcd2reg(vcd, (time, name, value) => {
    switch (name) {
        case 'SPH':
            stackSPH = value;
            updateStack(time, value, 'SPL', 'SPH');
            break;

        case 'SPL':
            stackSPL = value;
            updateStack(time, value, 'SPH', 'SPL');
            break;

        case 'DAT':
            data.push(value);
            break;

        case 'CMD':
            executeCommand(value, data);
            data = [];
            break;
    }
})
