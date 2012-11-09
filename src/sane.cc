#include <node.h>
#include <node_buffer.h>
#include <node_version.h>
#include <sane/sane.h>
#include <v8.h>
#include "sane_device.h"

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
	/*
	if (status != SANE_STATUS_GOOD) {
		const SANE_String_Const status_str = sane_strstatus (status);
		return ThrowException (Exception::Error (String::New (status_str)));
	}
	*/

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

void init (Handle<Object> target) {
	NODE_SET_METHOD (target, "init", Init);
	NODE_SET_METHOD (target, "exit", Exit);
	NODE_SET_METHOD (target, "getDevices", GetDevices);
}

NODE_MODULE (sane, init)

