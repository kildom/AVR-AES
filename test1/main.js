
const fs = require("fs");

const { vcd2msg } = require('./mod/vcd');
const { calcMapSizes } = require('./mod/map');

console.log(calcMapSizes('test1.map', obj => (obj.file == "aes.o")));


/*vcd2msg('aes_test.vcd', (time, name, data) => {
    console.log(time, name, data);
});*/
