'use strict';
var sane = require('../index.js');
sane.init();

if (process.argv.length < 3) {
  console.log('Usage: node ./get-scanner-options.js <name>');
} else {
  sane.open(process.argv[2], function (device) {
    if (device.status !== sane.Status.GOOD) {
      console.log('Unable to open device: status code %d', device.status);
      return;
    }

    var i = 0;
    var option;

    do {
      option = sane.getOptionDescriptor(device.handle, i);
      i++;
      if (option) { console.log(option); }

    } while (option);
  });
}

