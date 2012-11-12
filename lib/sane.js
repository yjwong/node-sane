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

sane.ValueType = {
	BOOL   : 0,
	INT    : 1,
	FIXED  : 2,
	STRING : 3,
	BUTTON : 4,
	GROUP  : 5
}
Object.freeze(sane.ValueType);

sane.Unit = {
	NONE        : 0,
	PIXEL       : 1,
	BIT         : 2,
	MM          : 3,
	DPI         : 4,
	PERCENT     : 5,
	MICROSECOND : 6
}
Object.freeze(sane.Unit);

sane.ConstraintType = {
	NONE        : 0,
	RANGE       : 1,
	WORD_LIST   : 2,
	STRING_LIST : 3
}
Object.freeze(sane.ConstraintType);

