#include "sane_device.h"

using namespace v8;
using namespace node;

SaneDevice::SaneDevice (const SANE_Device * device) :
		_device (device) {
}

SaneDevice::~SaneDevice () {
}

Handle<Value>
SaneDevice::New (const SANE_Device * device) {
	SaneDevice * device_wrap = new SaneDevice (device);

	Handle<FunctionTemplate> func_tpl = FunctionTemplate::New ();
	func_tpl->SetClassName (String::New ("SaneDevice"));
	func_tpl->InstanceTemplate ()->SetInternalFieldCount (1);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("name"), GetName);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("vendor"), GetVendor);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("model"), GetModel);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("type"), GetType);

	Handle<Object> object = func_tpl->InstanceTemplate ()->NewInstance ();
	object->SetInternalField (0, External::New (device_wrap));
	return object;
}

Handle<Value>
SaneDevice::GetName (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (String::New (ObjectWrap::Unwrap<SaneDevice> (info.Holder ())->_device->name));
}

Handle<Value>
SaneDevice::GetVendor (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (String::New (ObjectWrap::Unwrap<SaneDevice> (info.Holder ())->_device->vendor));
}

Handle<Value>
SaneDevice::GetModel (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (String::New (ObjectWrap::Unwrap<SaneDevice> (info.Holder ())->_device->model));
}

Handle<Value>
SaneDevice::GetType (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (String::New (ObjectWrap::Unwrap<SaneDevice> (info.Holder ())->_device->type));
}

