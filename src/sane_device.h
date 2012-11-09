#ifndef SANE_DEVICE_H
#define SANE_DEVICE_H

#include <sane/sane.h>
#include <node.h>

class SaneDevice : public node::ObjectWrap {
public:
	SaneDevice (const SANE_Device * device);
	~SaneDevice ();

	static v8::Handle<v8::Value> New (const SANE_Device * device);

private:
	const SANE_Device * _device;

	static v8::Handle<v8::Value> GetName (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetVendor (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetModel (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetType (v8::Local<v8::String> property, const v8::AccessorInfo& info);
};

#endif /* SANE_DEVICE_H */
