#include "sane_handle.h"

using namespace v8;
using namespace node;

SaneHandle::SaneHandle (SANE_Handle handle) :
		_handle (handle) {
}

SaneHandle::~SaneHandle () {
}

Handle<Value>
SaneHandle::New (SANE_Handle handle) {
	HandleScope scope;

	Handle<FunctionTemplate> func_tpl = FunctionTemplate::New ();
	func_tpl->SetClassName (String::New ("SaneHandle"));
	func_tpl->InstanceTemplate ()->SetInternalFieldCount (1);

	Handle<Object> object = func_tpl->InstanceTemplate ()->NewInstance ();
	object->SetInternalField (0, External::New (handle));

	return scope.Close (object);
}

