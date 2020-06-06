const fs = require("fs");

const CMD = {
    TIMER_START: 1,
    TIMER_STOP: 2,
    NAME: 3,
    UINT: 4,
    STR: 5,
    STACK: -1
};

let vcd = fs.readFileSync('aes_test.vcd', 'UTF-8');


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
            callback(now, nameOf[char], parseInt(bits, 2));
        }
        return '';
    });
}


function vcd2cmd(vcd, callback) {

    let stack = 0;
    let stackSPL = 0;
    let stackSPH = 0;
    let stackLast = '';
    let stackTime = 0;
    let data = [];

    function updateStack(time, value, expected, current) {
        if (stackLast == expected) {
            stack = stackSPL | stackSPH << 8;
            callback(time, CMD.STACK, [stack, time - stackTime]);
            stackLast = '';
        } else if (stackLast == current) {
            throw Error('Unexpected sequence of SPL/SPH. Unable to correctly interpret stack size.');
        } else {
            stackLast = current;
        }
        stackValue = value;
        stackTime = time;
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
                callback(time, value, data);
                data = [];
                break;
        }
    });
}


function vcd2msg(vcd, callback) {

    let currentStack = 0;
    let lastName = '';
    let timerInfo = [];
    
    function arrToString(arr) {
        let str = '';
        for (let i = 0; i < arr.length; i++)
            str += String.fromCharCode(arr[i]);
        return str;
    }

    function arrToUint(arr) {
        let uint = 0;
        let sh = 0;
        for (let i = 0; i < arr.length; i++) {
            uint |= arr[i] << sh;
            sh += 8;
        }
        return uint;
    }
    
    vcd2cmd(vcd, (time, id, data) => {
        switch (id) {
            case CMD.TIMER_START:
                timerInfo.push({
                    name: lastName,
                    stackMin: currentStack,
                    stackMax: currentStack,
                    time: time
                });
                break;

            case CMD.TIMER_STOP:
                if (timerInfo.length == 0) throw Error("Invalid timer start/stop sequence.");
                let info = timerInfo.pop();
                info.time = Math.max(0, time - info.time);
                callback(time, info.name, info);
                break;

            case CMD.NAME:
                lastName = arrToString(data);
                break;

            case CMD.UINT:
                data = arrToUint(data);
                callback(time, lastName, data);
                break;

            case CMD.STR:
                data = arrToString(data);
                callback(time, lastName, data);
                break;

            case CMD.STACK:
                currentStack = data[0];
                for (let i = 0; i < timerInfo.length; i++) {
                    timerInfo[i].stackMax = Math.max(timerInfo[i].stackMax, currentStack);
                    timerInfo[i].stackMin = Math.min(timerInfo[i].stackMin, currentStack);
                }
                break;
        }
    });
}


let frequency = 20000000;
let cyclePeriod = 50;
let cycleMul = 1 / 50;

function executeParam(time, name, value) {
    console.log(time, name, value);
    switch (name) {
        case 'frequency':
            frequency = value;
            cyclePeriod = 1 / value * 1000000000;
            cycleMul = 1 / cyclePeriod;
            break;
    }
}



vcd2msg(vcd, (time, name, data) => {
    console.log(time, name, data);
});
