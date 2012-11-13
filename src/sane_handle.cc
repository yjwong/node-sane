#include "sane_handle.h"

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> SaneHandle::function_template;

void
SaneHandle::Init (Handle<Object> target) {
	HandleScope scope;

	// Set up function template.
	Handle<FunctionTemplate> t = FunctionTemplate::New ();
	function_template = Persistent<FunctionTemplate>::New (t);
	function_template->SetClassName (String::NewSymbol ("SaneHandle"));

	// Obtain the instance template.
	Local<ObjectTemplate> instance_template = function_template->InstanceTemplate ();
	instance_template->SetInternalFieldCount (1);
}

Handle<Value>
SaneHandle::Wrap (SANE_Handle handle) {
	HandleScope scope;

	// Create a new instance.
	Local<Object> instance = function_template->InstanceTemplate ()->NewInstance ();	
	instance->SetInternalField (0, External::New (handle));
	
	return scope.Close (instance);
}

