const fs = require("fs");

function parseMap(fileName) {

    let map = fs.readFileSync(fileName, "UTF-8").replace(/\r/g, "");
    let result = [];

    map.replace(/\n\s*((?:\..+?)|COMMON)\s+0x([0-9A-Fa-z]{8})[\t ]+0x([0-9A-Fa-z]{1,8})[ \t]+([^\n]+)((?:\n[\t ]+0x[0-9a-fA-F]{8}[\t ]+(?:0x[0-9a-fA-F]{1,8}[\t ]+)?[^\n]*)*)/g, (_m, sec, addr, size, file, symbols) => {
        let obj = {
            name: sec,
            addr: parseInt(addr, 16),
            size: parseInt(size, 16),
            file: file,
            symbols: []
        };
        obj.symbols = symbols
            .trim()
            .split(/\s*\n\s*/g)
            .filter(v => (v != ''))
            .map(v => {
                let m = v.match(/^0x([0-9a-fA-F]{1,8})\s+(?:0x[0-9a-fA-F]{1,8}\s+)?(.+)$/);
                if (!m) throw Error("Invalid symbol");
                return {
                    addr: parseInt(m[1], 16),
                    name: m[2].trim()
                };
            });
        //console.log(JSON.stringify(obj, null, 4));
        result.push(obj);
        return '';
    });
    return result;
};

function calcMapSizes(fileName, filter) {
    let result = {
        flash: 0,
        ram: 0,
        flashRemoved: 0,
        ramRemoved: 0,
    };
    parseMap(fileName)
        .filter(obj => {
            return !obj.name.match(/^\.debug|comment/);
        })
        .filter(filter)
        .forEach(obj => {
            if (obj.name.match(/^\.text|^\.progmem/)) {
                if (obj.addr == 0) {
                    result.flashRemoved += obj.size;
                } else {
                    result.flash += obj.size;
                }
            } else if (obj.name.match(/^\.data/)) {
                if (obj.addr == 0) {
                    result.flashRemoved += obj.size;
                    result.ramRemoved += obj.size;
                } else {
                    result.flash += obj.size;
                    result.ram += obj.size;
                }
            } else if (obj.name.match(/^\.bss|^COMMON/)) {
                if (obj.addr == 0) {
                    result.ramRemoved += obj.size;
                } else {
                    result.ram += obj.size;
                }
            } else {
                console.log(JSON.stringify(obj, null, 4));
            }
        });
    return result;
}

exports.calcMapSizes = calcMapSizes;
