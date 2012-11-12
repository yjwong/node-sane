#include <iostream>
#include <string>
#include <node.h>
#include <node_buffer.h>
#include <node_version.h>
#include <sane/sane.h>
#include <v8.h>
#include "sane_device.h"
#include "sane_handle.h"
#include "sane_option_descriptor.h"

using namespace v8;
using namespace node;

// These things need to be moved into a class, so each instance can have its own session.
// Right now, there can only be one SANE instance at a time.
SANE_Int version_code;

Handle<Value>
Init (const Arguments& args) {
	HandleScope scope;

	SANE_Status status;
	status = sane_init (&version_code, NULL); // TODO: Set authorization callback.
	return scope.Close (Number::New (status));
}

Handle<Value>
Exit (const Arguments& args) {
	sane_exit ();
	return Undefined ();
}

struct GetDevicesBaton {
	uv_work_t request;
	Persistent<Function> callback;
	bool local_only;

	// Resultant data
	SANE_Status status;
	const SANE_Device** device_list;
};

void GetDevicesAsyncWork (uv_work_t* req) {
	GetDevicesBaton* baton = static_cast<GetDevicesBaton*> (req->data);
	baton->status = sane_get_devices (&baton->device_list, baton->local_only);
}

void GetDevicesAsyncAfter (uv_work_t* req) {
	HandleScope scope;
	GetDevicesBaton* baton = static_cast<GetDevicesBaton*> (req->data);

	if (baton->status == SANE_STATUS_GOOD) {
		Local<Array> device_list = Array::New ();
		for (int i = 0; baton->device_list[i]; i++) {
			device_list->Set (i, SaneDevice::New (baton->device_list[i]));
		}

		Handle<Value> argv[] = { device_list };
		baton->callback->Call (Context::GetCurrent ()->Global (),
			1, argv);

	} else {
		Local<Number> status = Number::New (baton->status);
		Handle<Value> argv[] = { status };
		baton->callback->Call (Context::GetCurrent ()->Global (),
			1, argv);
	}

	baton->callback.Dispose ();
	delete baton;
}

Handle<Value>
GetDevices (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 2) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 2 arguments")));
	}

	if (!args[0]->IsFunction ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a callback function")));
	}

	if (!args[1]->IsBoolean ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be a boolean")));
	}

	Local<Function> callback = Local<Function>::Cast (args[0]);
	Local<Boolean> local_only = args[1]->ToBoolean ();

	GetDevicesBaton* baton = new GetDevicesBaton ();
	baton->request.data = baton;
	baton->callback = Persistent<Function>::New (callback);
	baton->local_only = local_only->Value ();

	uv_queue_work (uv_default_loop (), &baton->request,
		GetDevicesAsyncWork, GetDevicesAsyncAfter);

	return Undefined ();
}

struct OpenBaton {
	uv_work_t request;
	Persistent<Function> callback;
	SANE_String_Const name;

	// Resultant data
	SANE_Handle handle;
	SANE_Status status;
};

void OpenAsyncWork (uv_work_t* req) {
	OpenBaton* baton = static_cast<OpenBaton*> (req->data);
	baton->status = sane_open (baton->name, &baton->handle);
	delete[] baton->name;
}

void OpenAsyncAfter (uv_work_t* req) {
	HandleScope scope;
	OpenBaton* baton = static_cast<OpenBaton*> (req->data);

	if (baton->status == SANE_STATUS_GOOD) {
		Handle<Value> handle = SaneHandle::New (baton->handle);
		Handle<Value> argv[] = { handle };
		baton->callback->Call (Context::GetCurrent ()->Global(),
			1, argv);

	} else {
		Local<Number> status = Number::New (baton->status);
		Handle<Value> argv[] = { status };
		baton->callback->Call (Context::GetCurrent ()->Global (),
			1, argv);
	}

	baton->callback.Dispose ();
	delete[] baton;
}

Handle<Value>
Open (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 2) {
		return ThrowException (Exception::TypeError (String::New (
			"Threre should be exactly 1 argument")));
	}

	if (!args[0]->IsFunction ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a callback")));
	}

	if (!args[1]->IsString ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be a string")));
	}

	Local<Function> callback = Local<Function>::Cast (args[0]);
	String::AsciiValue name (args[1]->ToString ());
	char* name_str = new char[strlen (*name)];
	strcpy (name_str, *name);

	OpenBaton* baton = new OpenBaton ();
	baton->request.data = baton;
	baton->callback = Persistent<Function>::New (callback);
	baton->name = name_str;

	uv_queue_work (uv_default_loop (), &baton->request,
		OpenAsyncWork, OpenAsyncAfter);

	return Undefined ();
}

Handle<Value>
Close (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 1) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 1 argument")));
	}

	if (!args[0]->IsObject ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an object")));
	}

	Local<Object> handle = args[0]->ToObject ();
	void* ptr = External::Unwrap (handle->GetInternalField (0));
	
	sane_close (ptr);
	return scope.Close (Undefined ());
}

Handle<Value>
GetOptionDescriptor (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 2) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 2 arguments")));
	}

	if (!args[0]->IsObject ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an object")));
	}

	if (!args[1]->IsInt32 ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be an integer")));
	}

	Local<Object> handle = args[0]->ToObject ();
	void* ptr = External::Unwrap (handle->GetInternalField (0));
	Local<Integer> n = args[1]->ToInteger ();
	
	const SANE_Option_Descriptor * option = sane_get_option_descriptor (
		ptr, n->Value ());
	if (option == NULL) {
		return scope.Close (Undefined ());
	}
	
	return scope.Close (SaneOptionDescriptor::New (option));
}


Handle<Value>
ControlOption (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 4) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 5 arguments")));
	}

	if (!args[0]->IsObject ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an object")));
	}

	if (!args[1]->IsInt32 ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be an integer")));
	}

	if (!args[2]->IsInt32 ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Third argument must be an integer")));
	}

	if (!Buffer::HasInstance(args[3])) {
		return ThrowException (Exception::TypeError (String::New (
			"Fourth argument must be a buffer")));
	}

	Local<Object> handle = args[0]->ToObject ();
	void* ptr = External::Unwrap (handle->GetInternalField (0));
	Local<Integer> n = args[1]->ToInteger ();
	Local<Integer> a = args[2]->ToInteger ();
	char* v = Buffer::Data (args[3]->ToObject ());

	SANE_Status status;
	SANE_Int i;
	status = sane_control_option (ptr, n->Value (), (SANE_Action) a->Value (), v, &i);

	return scope.Close (Number::New (status));
}

void init (Handle<Object> target) {
	NODE_SET_METHOD (target, "init", Init);
	NODE_SET_METHOD (target, "exit", Exit);
	NODE_SET_METHOD (target, "getDevices", GetDevices);
	NODE_SET_METHOD (target, "open", Open);
	NODE_SET_METHOD (target, "close", Close);
	NODE_SET_METHOD (target, "getOptionDescriptor", GetOptionDescriptor);
	NODE_SET_METHOD (target, "controlOption", ControlOption);
}

NODE_MODULE (sane, init)

