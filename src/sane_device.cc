#include "sane_device.h"

using v8::Local;
using v8::String;
using v8::Handle;
using v8::Value;
using v8::Function;
using v8::FunctionTemplate;
using v8::Object;
using v8::External;

Nan::Persistent<FunctionTemplate> SaneDevice::constructor_template;

SaneDevice::SaneDevice(const SANE_Device* device) :
		_device(device) {
}

SaneDevice::~SaneDevice() {
}

NAN_MODULE_INIT(SaneDevice::Init) {
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("SaneDevice").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("name").ToLocalChecked(), GetName);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("vendor").ToLocalChecked(), GetVendor);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("model").ToLocalChecked(), GetModel);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("type").ToLocalChecked(), GetType);

    constructor_template.Reset(tpl);
    target->Set(Nan::New("SaneDevice").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(SaneDevice::New) {
    Nan::HandleScope scope;

    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new SaneDevice objects");
    }

    if (info.Length() < 1 || !info[0]->IsExternal()) {
        return Nan::ThrowTypeError("SaneDevice object cannot be created directly");
    }

    SaneDevice* device = new SaneDevice(static_cast<SANE_Device*>(
        External::Cast(*info[0])->Value()));
    device->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

NAN_GETTER(SaneDevice::GetName) {
    Nan::HandleScope scope;
    SaneDevice* device = Nan::ObjectWrap::Unwrap<SaneDevice>(info.This());
    info.GetReturnValue().Set(Nan::New(device->_device->name).ToLocalChecked());
}

NAN_GETTER(SaneDevice::GetVendor) {
    Nan::HandleScope scope;
    SaneDevice* device = Nan::ObjectWrap::Unwrap<SaneDevice>(info.This());
    info.GetReturnValue().Set(Nan::New(device->_device->vendor).ToLocalChecked());
}

NAN_GETTER(SaneDevice::GetModel) {
    Nan::HandleScope scope;
    SaneDevice* device = Nan::ObjectWrap::Unwrap<SaneDevice>(info.This());
    info.GetReturnValue().Set(Nan::New(device->_device->model).ToLocalChecked());
}

NAN_GETTER(SaneDevice::GetType) {
    Nan::HandleScope scope;
    SaneDevice* device = Nan::ObjectWrap::Unwrap<SaneDevice>(info.This());
    info.GetReturnValue().Set(Nan::New(device->_device->type).ToLocalChecked());
}

