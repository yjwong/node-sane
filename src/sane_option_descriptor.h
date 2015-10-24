#ifndef SANE_OPTION_DESCRIPTOR_H
#define SANE_OPTION_DESCRIPTOR_H

#include <sane/sane.h>
#include <node.h>
#include "nan.h"

class SaneOptionDescriptor : public Nan::ObjectWrap {
public:
	explicit SaneOptionDescriptor(const SANE_Option_Descriptor* descriptor);
	~SaneOptionDescriptor();

	static Nan::Persistent<v8::FunctionTemplate> constructor_template;
	static NAN_MODULE_INIT(Init);

private:
    static NAN_METHOD(New);

	static NAN_GETTER(GetName);
	static NAN_GETTER(GetTitle);
	static NAN_GETTER(GetDesc);
	static NAN_GETTER(GetType);
	static NAN_GETTER(GetUnit);
	static NAN_GETTER(GetSize);
	static NAN_GETTER(GetCap);
	static NAN_GETTER(GetConstraintType);
	//static NAN_GETTER(GetConstraint);
	
	const SANE_Option_Descriptor* _descriptor;
};

#endif /* SANE_OPTION_DESCRIPTOR_H */
