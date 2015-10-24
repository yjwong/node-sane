#include "sane_parameters.h"

using v8::Persistent;
using v8::FunctionTemplate;
using v8::Local;
using v8::External;
using v8::Integer;
using v8::Boolean;

Nan::Persistent<FunctionTemplate> SaneParameters::constructor_template;

NAN_MODULE_INIT(SaneParameters::Init) {
    Nan::HandleScope scope;

	// Set up function template
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("SaneParameters").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Set instance template parameter
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("format").ToLocalChecked(), GetFormat, SetFormat);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("lastFrame").ToLocalChecked(), GetLastFrame, SetLastFrame);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("bytesPerLine").ToLocalChecked(), GetBytesPerLine, SetBytesPerLine);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("pixelsPerLine").ToLocalChecked(), GetPixelsPerLine, SetPixelsPerLine);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("lines").ToLocalChecked(), GetLines, SetLines);
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("depth").ToLocalChecked(), GetDepth, SetDepth);

	// Make it visible in JavaScript
    constructor_template.Reset(tpl);
    target->Set(Nan::New("SaneParameters").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(SaneParameters::New) {
    Nan::HandleScope scope;
	SANE_Parameters* parameters = new SANE_Parameters();
	info.Holder()->SetInternalField(0, Nan::New<External>(parameters));
    info.GetReturnValue().Set(info.Holder());
}

NAN_GETTER(SaneParameters::GetFormat) {
    Nan::HandleScope scope;
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
    info.GetReturnValue().Set(Nan::New(parameters->format));
}

NAN_SETTER(SaneParameters::SetFormat) {
    Nan::HandleScope scope;
	if (!value->IsInt32()) {
        return Nan::ThrowTypeError("format must be an integer");
	}

	Local<Integer> format = value.As<Integer>();
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
	parameters->format = (SANE_Frame) format->Value();
}

NAN_GETTER(SaneParameters::GetLastFrame) {
    Nan::HandleScope scope;
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
    info.GetReturnValue().Set(Nan::New(parameters->last_frame));
}

NAN_SETTER(SaneParameters::SetLastFrame) {
    Nan::HandleScope scope;
	if (!value->IsBoolean()) {
		return Nan::ThrowTypeError("lastFrame must be a boolean");
	}

	Local<Boolean> lastFrame = Nan::To<Boolean>(value).ToLocalChecked();
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
	parameters->last_frame = lastFrame->Value();
}

NAN_GETTER(SaneParameters::GetBytesPerLine) {
    Nan::HandleScope scope;
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
    info.GetReturnValue().Set(Nan::New(parameters->bytes_per_line));
}

NAN_SETTER(SaneParameters::SetBytesPerLine) {
    Nan::HandleScope scope;
	if (!value->IsInt32()) {
		return Nan::ThrowTypeError("bytesPerLine must be an integer");
	}

	Local<Integer> bytesPerLine = value.As<Integer>();
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
	parameters->bytes_per_line = bytesPerLine->Value();
}

NAN_GETTER(SaneParameters::GetPixelsPerLine) {
    Nan::HandleScope scope;
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
    info.GetReturnValue().Set(Nan::New(parameters->pixels_per_line));
}

NAN_SETTER(SaneParameters::SetPixelsPerLine) {
    Nan::HandleScope scope;
	if (!value->IsInt32()) {
		return Nan::ThrowTypeError("pixelsPerLine must be an integer");
	}

	Local<Integer> pixelsPerLine = value.As<Integer>();
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
	parameters->pixels_per_line = pixelsPerLine->Value();
}

NAN_GETTER(SaneParameters::GetLines) {
    Nan::HandleScope scope;
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
    info.GetReturnValue().Set(Nan::New(parameters->lines));
}

NAN_SETTER(SaneParameters::SetLines) {
    Nan::HandleScope scope;
	if (!value->IsInt32()) {
		return Nan::ThrowTypeError("lines must be an integer");
	}

	Local<Integer> lines = value.As<Integer>();
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
	parameters->lines = lines->Value();
}

NAN_GETTER(SaneParameters::GetDepth) {
    Nan::HandleScope scope;
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
    info.GetReturnValue().Set(Nan::New(parameters->depth));
}

NAN_SETTER(SaneParameters::SetDepth) {
    Nan::HandleScope scope;
	if (!value->IsInt32()) {
		return Nan::ThrowTypeError("depth must be an integer");
	}

	Local<Integer> depth = value.As<Integer>();
    Local<External> parametersExt = info.Holder()->GetInternalField(0).As<External>();
	SANE_Parameters* parameters = static_cast<SANE_Parameters*>(parametersExt->Value());
	parameters->depth = depth->Value();
}

