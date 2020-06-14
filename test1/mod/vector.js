const fs = require("fs");

function readAll() {
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
            vector[names[i]] = items[i];
            if (items[i] != '-') name.push(`${names[i].replace(/^AES_/, '')}=${items[i]}`);
        }
        vectors.push({
            name: name.join('; '),
            vector: vector
        });
    }
    return vectors;
}

function write(testCase) {
    let h = fs.readFileSync('../aes.h', 'UTF-8');
    fs.renameSync('../aes.h', '../_aes_copy.h_');
    for (let name in testCase.vector) {
        let value = testCase.vector[name];
        let h2 = h.replace(new RegExp(`(\n#define\\s+${name}\\s+)(.+?)(\r?\n)`), `$1((((${value.replace('-', '_undefined_')}))))$3`);
        if (h2 == h) throw Error(`Definition '${name}'`);
        h = h2;
    }
    fs.writeFileSync('../aes.h', h);
}

function restore() {
    fs.unlinkSync('../aes.h');
    fs.renameSync('../_aes_copy.h_', '../aes.h');
}

exports.readAll = readAll;
exports.write = write;
exports.restore = restore;
