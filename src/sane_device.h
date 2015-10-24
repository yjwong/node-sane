#ifndef SANE_DEVICE_H
#define SANE_DEVICE_H

#include <sane/sane.h>
#include <node.h>
#include "nan.h"

class SaneDevice : public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor_template;
    static NAN_MODULE_INIT(Init);

private:
    explicit SaneDevice(const SANE_Device* device);
	~SaneDevice();

    static NAN_METHOD(New);
	static NAN_GETTER(GetName);
	static NAN_GETTER(GetVendor);
	static NAN_GETTER(GetModel);
	static NAN_GETTER(GetType);
	
    const SANE_Device* _device;
};

#endif /* SANE_DEVICE_H */
