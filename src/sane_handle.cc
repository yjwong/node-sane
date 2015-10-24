#include "sane_handle.h"

using v8::FunctionTemplate;
using v8::Local;
using v8::External;

Nan::Persistent<FunctionTemplate> SaneHandle::constructor_template;

NAN_MODULE_INIT(SaneHandle::Init) {
    Nan::HandleScope scope;

    // Prepare constructor template.
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("SaneHandle").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    constructor_template.Reset(tpl);
    target->Set(Nan::New("SaneHandle").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(SaneHandle::New) {
    Nan::HandleScope scope;

    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new SaneHandle objects");
    }

    if (info.Length() < 1 || !info[0]->IsExternal()) {
        return Nan::ThrowTypeError("SaneHandle object cannot be created directly");
    }

    SaneHandle* handle = new SaneHandle(static_cast<SANE_Handle>(
        External::Cast(*info[0])->Value()));
    handle->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

SANE_Handle SaneHandle::getHandle() {
    return _handle;
}

