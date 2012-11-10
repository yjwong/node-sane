var sane = require('../build/Release/sane');
module.exports = sane;

sane.Status = {
	GOOD          : 0,
	UNSUPPORTED   : 1,
	CANCELLED     : 2,
	DEVICE_BUSY   : 3,
	INVAL         : 4,
	EOF           : 5,
	JAMMED        : 6,
	NO_DOCS       : 7,
	COVER_OPEN    : 8,
	IO_ERROR      : 9,
	NO_MEM        : 10,
	ACCESS_DENIED : 11
}
Object.freeze(sane.Status);

