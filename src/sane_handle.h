#ifndef SANE_HANDLE_H
#define SANE_HANDLE_H

#include <sane/sane.h>
#include <node.h>
#include "nan.h"

class SaneHandle : public Nan::ObjectWrap {
public:
	static Nan::Persistent<v8::FunctionTemplate> constructor_template;
	static NAN_MODULE_INIT(Init);
    SANE_Handle getHandle();

private:
	explicit SaneHandle(SANE_Handle handle) : _handle(handle) { };
	~SaneHandle() { };

    static NAN_METHOD(New);

	SANE_Handle _handle;
};

#endif /* SANE_HANDLE_H */
