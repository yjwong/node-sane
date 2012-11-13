#include "sane_parameters.h"

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> SaneParameters::constructor_template;

void
SaneParameters::Init (Handle<Object> target) {
	HandleScope scope;

	// Set up function template
	Local<FunctionTemplate> t = FunctionTemplate::New (SaneParameters::New);

	// Constructor template
	constructor_template = Persistent<FunctionTemplate>::New (t);
	constructor_template->SetClassName (String::NewSymbol ("SaneParameters"));

	// Instance template
	Local<ObjectTemplate> instance_template = constructor_template->InstanceTemplate ();
	instance_template->SetInternalFieldCount (1);

	// Set instance template parameters
	instance_template->SetAccessor (String::New ("format"), GetFormat, SetFormat);
	instance_template->SetAccessor (String::New ("lastFrame"), GetLastFrame, SetLastFrame);
	instance_template->SetAccessor (String::New ("bytesPerLine"), GetBytesPerLine, SetBytesPerLine);
	instance_template->SetAccessor (String::New ("pixelsPerLine"), GetPixelsPerLine, SetPixelsPerLine);
	instance_template->SetAccessor (String::New ("lines"), GetLines, SetLines);
	instance_template->SetAccessor (String::New ("depth"), GetDepth, SetDepth);

	// Make it visible in JavaScript
	target->Set (String::NewSymbol ("SaneParameters"), constructor_template->GetFunction ());
}

Handle<Value>
SaneParameters::New (const Arguments& args) {
	HandleScope scope;
	
	SANE_Parameters * parameters = new SANE_Parameters ();
	args.Holder ()->SetInternalField (0, External::New (parameters));
	return args.Holder ();
}

Handle<Value>
SaneParameters::Wrap (SANE_Parameters * parameters) {
	HandleScope scope;

	// Create a new instance.
	Local<Object> instance = constructor_template->InstanceTemplate ()->NewInstance ();
	instance->SetInternalField (0, External::New (parameters));
	
	return scope.Close (instance);
}

Handle<Value>
SaneParameters::GetFormat (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder());
	return scope.Close (Integer::New (parameters->format));
	//return scope.Close (Integer::New (ObjectWrap::Unwrap<SaneParameters> (info.Holder ())->_parameters->format));
}

void
SaneParameters::SetFormat (Local<String> property, Local<Value> value,
		const AccessorInfo& info) {
	HandleScope scope;
	if (!value->IsInt32 ()) {
		ThrowException (Exception::TypeError (String::New (
			"format must be an integer")));
		return;
	}

	Local<Integer> format = value->ToInteger ();
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	parameters->format = (SANE_Frame) format->Value ();
}

Handle<Value>
SaneParameters::GetLastFrame (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	return scope.Close (Boolean::New (parameters->last_frame));
}

void
SaneParameters::SetLastFrame (Local<String> property, Local<Value> value,
		const AccessorInfo& info) {
	HandleScope scope;
	if (!value->IsBoolean ()) {
		ThrowException (Exception::TypeError (String::New (
			"lastFrame must be a boolean")));
		return;
	}

	Local<Boolean> last_frame = value->ToBoolean ();
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	parameters->last_frame = last_frame->Value ();
}

Handle<Value>
SaneParameters::GetBytesPerLine (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	return scope.Close (Integer::New (parameters->bytes_per_line));
}

void
SaneParameters::SetBytesPerLine (Local<String> property, Local<Value> value,
		const AccessorInfo& info) {
	HandleScope scope;
	if (!value->IsInt32 ()) {
		ThrowException (Exception::TypeError (String::New (
			"bytesPerLine must be an integer")));
		return;
	}

	Local<Integer> bytes_per_line = value->ToInteger ();
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	parameters->bytes_per_line = bytes_per_line->Value ();
}

Handle<Value>
SaneParameters::GetPixelsPerLine (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	return scope.Close (Integer::New (parameters->pixels_per_line));
}

void
SaneParameters::SetPixelsPerLine (Local<String> property, Local<Value> value,
		const AccessorInfo& info) {
	HandleScope scope;
	if (!value->IsInt32 ()) {
		ThrowException (Exception::TypeError (String::New (
			"pixelsPerLine must be an integer")));
		return;
	}

	Local<Integer> pixels_per_line = value->ToInteger ();
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	parameters->pixels_per_line = pixels_per_line->Value ();
}


Handle<Value>
SaneParameters::GetLines (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	return scope.Close (Integer::New (parameters->lines));
}

void
SaneParameters::SetLines (Local<String> property, Local<Value> value,
		const AccessorInfo& info) {
	HandleScope scope;
	if (!value->IsInt32 ()) {
		ThrowException (Exception::TypeError (String::New (
			"lines must be an integer")));
		return;
	}

	Local<Integer> lines = value->ToInteger ();
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	parameters->lines = lines->Value ();
}

Handle<Value>
SaneParameters::GetDepth (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	return scope.Close (Integer::New (parameters->depth));
}

void
SaneParameters::SetDepth (Local<String> property, Local<Value> value,
		const AccessorInfo& info) {
	HandleScope scope;
	if (!value->IsInt32 ()) {
		ThrowException (Exception::TypeError (String::New (
			"depth must be an integer")));
		return;
	}

	Local<Integer> depth = value->ToInteger ();
	SANE_Parameters * parameters = ObjectWrap::Unwrap<SANE_Parameters> (info.Holder ());
	parameters->depth = depth->Value ();
}

