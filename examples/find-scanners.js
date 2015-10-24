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

/*
sane.open('test:0', function (device) {
  var i = 0;
  var options;
  while (options = sane.getOptionDescriptor(device.handle, i)) {
    console.log(options);
    i++;
  }
});
*/
