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

	Handle<ObjectTemplate> device_tpl = ObjectTemplate::New ();
	device_tpl->SetInternalFieldCount (1);
	device_tpl->SetAccessor (String::New ("name"), GetName);
	device_tpl->SetAccessor (String::New ("vendor"), GetVendor);
	device_tpl->SetAccessor (String::New ("model"), GetModel);
	device_tpl->SetAccessor (String::New ("type"), GetType);

	Handle<Object> object = device_tpl->NewInstance ();
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

