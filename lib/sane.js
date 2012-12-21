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

sane.Cap = {
  SOFT_SELECT : 1,
  HARD_SELECT : 2,
  SOFT_DETECT : 4,
  EMULATED    : 8,
  AUTOMATIC   : 16,
  INACTIVE    : 32,
  ADVANCED    : 64
}
Object.freeze(sane.Cap);

sane.ConstraintType = {
  NONE        : 0,
  RANGE       : 1,
  WORD_LIST   : 2,
  STRING_LIST : 3
}
Object.freeze(sane.ConstraintType);

sane.Action = {
  GET_VALUE : 0,
  SET_VALUE : 1,
  SET_AUTO  : 2
}
Object.freeze(sane.Action);

sane.Info = {
  INEXACT        : 1,
  RELOAD_OPTIONS : 2,
  RELOAD_PARAMS  : 4
}
Object.freeze(sane.Info);

sane.Frame = {
  GRAY  : 0,
  RGB   : 1,
  RED   : 2,
  GREEN : 3,
  BLUE  : 4
}
Object.freeze(sane.Frame);

