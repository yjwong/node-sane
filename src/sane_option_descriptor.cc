#include "sane_option_descriptor.h"

using namespace v8;
using namespace node;

SaneOptionDescriptor::SaneOptionDescriptor (const SANE_Option_Descriptor * descriptor) :
		_descriptor (descriptor) {
}

SaneOptionDescriptor::~SaneOptionDescriptor () {
}

Handle<Value>
SaneOptionDescriptor::New (const SANE_Option_Descriptor * descriptor) {
	SaneOptionDescriptor * descriptor_wrap = new SaneOptionDescriptor (descriptor);

	Handle<FunctionTemplate> func_tpl = FunctionTemplate::New ();
	func_tpl->SetClassName (String::New ("SaneOptionDescriptor"));
	func_tpl->InstanceTemplate ()->SetInternalFieldCount (1);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("name"), GetName);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("title"), GetTitle);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("desc"), GetDesc);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("type"), GetType);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("unit"), GetUnit);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("size"), GetSize);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("cap"), GetCap);
	func_tpl->InstanceTemplate ()->SetAccessor (String::New ("constraintType"), GetConstraintType);
	//func_tpl->InstanceTemplate ()->SetAccessor (String::New ("constraint"), GetConstraint);
	
	Handle<Object> object = func_tpl->InstanceTemplate ()->NewInstance ();
	object->SetInternalField (0, External::New (descriptor_wrap));
	return object;
}

Handle<Value>
SaneOptionDescriptor::GetName (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (String::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->name));
}

Handle<Value>
SaneOptionDescriptor::GetTitle (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (String::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->title));
}

Handle<Value>
SaneOptionDescriptor::GetDesc (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (String::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->desc));
}

Handle<Value>
SaneOptionDescriptor::GetType (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (Integer::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->type));
}

Handle<Value>
SaneOptionDescriptor::GetUnit (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (Integer::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->unit));
}

Handle<Value>
SaneOptionDescriptor::GetSize (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (Integer::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->size));
}

Handle<Value>
SaneOptionDescriptor::GetCap (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (Integer::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->cap));
}

Handle<Value>
SaneOptionDescriptor::GetConstraintType (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	return scope.Close (Integer::New (ObjectWrap::Unwrap<SaneOptionDescriptor> (info.Holder ())->_descriptor->constraint_type));
}

