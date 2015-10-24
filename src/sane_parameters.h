#ifndef SANE_PARAMETERS_H
#define SANE_PARAMETERS_H

#include <sane/sane.h>
#include <node.h>
#include "nan.h"

class SaneParameters : public node::ObjectWrap {
public:
	SaneParameters(SANE_Parameters* parameters) : _parameters(parameters) { };
	~SaneParameters() { };
	
	static Nan::Persistent<v8::FunctionTemplate> constructor_template;
	static NAN_MODULE_INIT(Init);

private:
	static NAN_METHOD(New);

	static NAN_GETTER(GetFormat);
	static NAN_GETTER(GetLastFrame);
	static NAN_GETTER(GetBytesPerLine);
	static NAN_GETTER(GetPixelsPerLine);
	static NAN_GETTER(GetLines);
	static NAN_GETTER(GetDepth);

	static NAN_SETTER(SetFormat);
	static NAN_SETTER(SetLastFrame);
	static NAN_SETTER(SetBytesPerLine);
	static NAN_SETTER(SetPixelsPerLine);
	static NAN_SETTER(SetLines);
	static NAN_SETTER(SetDepth);

	SANE_Parameters* _parameters;
};

#endif /* SANE_PARAMETERS_H */
