#include "sane_option_descriptor.h"

using v8::Local;
using v8::FunctionTemplate;
using v8::External;

Nan::Persistent<FunctionTemplate> SaneOptionDescriptor::constructor_template;

SaneOptionDescriptor::SaneOptionDescriptor(const SANE_Option_Descriptor* descriptor) :
		_descriptor(descriptor) {
}

SaneOptionDescriptor::~SaneOptionDescriptor() {
}

NAN_MODULE_INIT(SaneOptionDescriptor::Init) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

    tpl->SetClassName(Nan::New("SaneOptionDescriptor").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("name").ToLocalChecked(), GetName);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("title").ToLocalChecked(), GetTitle);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("descr").ToLocalChecked(), GetDesc);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("type").ToLocalChecked(), GetType);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("unit").ToLocalChecked(), GetUnit);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("size").ToLocalChecked(), GetSize);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("cap").ToLocalChecked(), GetCap);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("constraintType").ToLocalChecked(), GetConstraintType);
    //Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("constraint").ToLocalChecked(), GetConstraint);
    
    constructor_template.Reset(tpl);
    target->Set(Nan::New("SaneOptionDescriptor").ToLocalChecked(), tpl->GetFunction()); 
}

NAN_METHOD(SaneOptionDescriptor::New) {
    Nan::HandleScope scope;

    if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create new SaneOptionDescriptor objects");
    }

    if (info.Length() < 1 || !info[0]->IsExternal()) {
        return Nan::ThrowTypeError("SaneOptionDescriptor object cannot be created directly");
    }

    SaneOptionDescriptor* descriptor = new SaneOptionDescriptor(static_cast<SANE_Option_Descriptor*>(
        External::Cast(*info[0])->Value()));
    descriptor->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
}

NAN_GETTER(SaneOptionDescriptor::GetName) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;

	if (descriptor->name == NULL) {
        info.GetReturnValue().Set(Nan::Undefined());
	} else {
        info.GetReturnValue().Set(Nan::New(descriptor->name).ToLocalChecked());
	}
}

NAN_GETTER(SaneOptionDescriptor::GetTitle) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;
    info.GetReturnValue().Set(Nan::New(descriptor->title).ToLocalChecked());
}

NAN_GETTER(SaneOptionDescriptor::GetDesc) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;
    info.GetReturnValue().Set(Nan::New(descriptor->desc).ToLocalChecked());
}

NAN_GETTER(SaneOptionDescriptor::GetType) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;
    info.GetReturnValue().Set(Nan::New(descriptor->type));
}

NAN_GETTER(SaneOptionDescriptor::GetUnit) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;
    info.GetReturnValue().Set(Nan::New(descriptor->unit));
}

NAN_GETTER(SaneOptionDescriptor::GetSize) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;
    info.GetReturnValue().Set(Nan::New(descriptor->size));
}

NAN_GETTER(SaneOptionDescriptor::GetCap) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;
    info.GetReturnValue().Set(Nan::New(descriptor->cap));
}

NAN_GETTER(SaneOptionDescriptor::GetConstraintType) {
    Nan::HandleScope scope;
	const SANE_Option_Descriptor* descriptor = 
		Nan::ObjectWrap::Unwrap<SaneOptionDescriptor>(info.Holder())->_descriptor;
    info.GetReturnValue().Set(Nan::New(descriptor->constraint_type));
}

