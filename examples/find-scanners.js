'use strict';
var sane = require('../index.js');
sane.init();

console.log('Finding scanners synchronously...');
var devices = sane.getDevicesSync(true);
console.log(devices);

console.log('Finding scanners asynchronously...');
sane.getDevices(true, function (devices) {
  console.log(devices);
});

