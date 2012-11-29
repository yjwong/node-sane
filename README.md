node-sane
=========
### Node.js Interface to SANE (Scanner Access Now Easy)

This is a set of bindings from `libsane` to node. `libsane` is a scanner access
library used to access scanners on supported systems.

Installation
------------

You will need `libsane` installed. Most Linux distributions have it available
by default, or with a package like `libsane-dev`.

The easiest way to get `node-sane` and its tools is with `npm`:

``` bash
$ npm install sane
```

If you want to hack on the source code, you can get it from github. Clone the
repo like this:

``` bash
$ git clone git://github.com/yjwong/node-sane.git
```

To compile the native code bindings, do this:

``` bash
$ cd node-sane
$ node-gyp configure build
```

Assuming it built without errors, you should be able to run the examples and
then write your own scanner access programs.

Usage
-----

There are several example programs that show how to use `node-sane`. These
examples are the best documentation. Try them out and see what they do.

To use this library in your own program, `sane.js` and `sane_binding.node` must
be in `NODE_PATH`. `npm` takes care of this automatically.

``` js
var sane = require('sane');
sane.init();
```

License
-------

MIT License. See the `LICENSE` file.

