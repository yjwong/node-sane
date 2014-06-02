#include <iostream>
#include <cstring>
#include <node.h>
#include <node_buffer.h>
#include <node_version.h>
#include <sane/sane.h>
#include <v8.h>
#include "sane_device.h"
#include "sane_handle.h"
#include "sane_option_descriptor.h"
#include "sane_parameters.h"

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
	HandleScope scope;
	sane_exit ();
	return scope.Close (Undefined ());
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

	Local<ObjectTemplate>  result = ObjectTemplate::New ();
	Local<Object> object = result->NewInstance ();
	object->Set (String::New ("status"), Integer::New (baton->status));
	
	if (baton->status == SANE_STATUS_GOOD) {
		Local<Array> device_list = Array::New ();
		for (int i = 0; baton->device_list[i]; i++) {
			device_list->Set (i, SaneDevice::New (baton->device_list[i]));
		}

		object->Set (String::New ("device_list"), device_list);
	}

	Handle<Value> argv[] = { object };
	baton->callback->Call (Context::GetCurrent ()->Global (),
		1, argv);

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

	if (!args[0]->IsBoolean ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a boolean")));
	}

	if (!args[1]->IsFunction ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a callback function")));
	}

	Local<Boolean> local_only = args[0]->ToBoolean ();
	Local<Function> callback = Local<Function>::Cast (args[1]);

	GetDevicesBaton* baton = new GetDevicesBaton ();
	baton->request.data = baton;
	baton->callback = Persistent<Function>::New (callback);
	baton->local_only = local_only->Value ();

	uv_queue_work (uv_default_loop (), &baton->request,
		GetDevicesAsyncWork, (uv_after_work_cb) GetDevicesAsyncAfter);

	return scope.Close (Undefined ());
}

Handle<Value>
GetDevicesSync (const Arguments& args) {
	HandleScope scope;

	if (args.Length() < 1) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 1 argument")));
	}

	if (!args[0]->IsBoolean ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a boolean")));
	}

	Local<Boolean> local_only = args[0]->ToBoolean ();

	const SANE_Device** device_list;
	SANE_Status status;
	status = sane_get_devices (&device_list, local_only->Value ());

	Local<ObjectTemplate> result = ObjectTemplate::New ();
	Local<Object> object = result->NewInstance ();
	object->Set (String::New ("status"), Integer::New (status));

	if (status == SANE_STATUS_GOOD) {
		Local<Array> device_list_arr = Array::New ();
		for (int i = 0; device_list[i]; i++) {
			device_list_arr->Set (i, SaneDevice::New (device_list[i]));
		}

		object->Set (String::New ("device_list"), device_list_arr);	
	}

	return scope.Close (object);
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

	Local<ObjectTemplate> result = ObjectTemplate::New ();
	Local<Object> object = result->NewInstance ();
	object->Set (String::New ("status"), Integer::New (baton->status));
	if (baton->status == SANE_STATUS_GOOD) {
		object->Set (String::New ("handle"), SaneHandle::Wrap (baton->handle));
	}

	Handle<Value> argv[] = { object };
	baton->callback->Call (Context::GetCurrent ()->Global(),
		1, argv);

	baton->callback.Dispose ();
	delete[] baton;
}

Handle<Value>
Open (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 2) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 2 arguments")));
	}

	if (!args[0]->IsString ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a string")));
	}

	if (!args[1]->IsFunction ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be a callback function")));
	}

	String::AsciiValue name (args[0]->ToString ());
	Local<Function> callback = Local<Function>::Cast (args[1]);
	char* name_str = new char[strlen (*name)];
	strcpy (name_str, *name);

	OpenBaton* baton = new OpenBaton ();
	baton->request.data = baton;
	baton->callback = Persistent<Function>::New (callback);
	baton->name = name_str;

	uv_queue_work (uv_default_loop (), &baton->request,
		OpenAsyncWork, (uv_after_work_cb) OpenAsyncAfter);

	return scope.Close (Undefined ());
}

Handle<Value>
OpenSync (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 1) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 1 argument")));
	}

	if (!args[0]->IsString ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a string")));
	}

	String::AsciiValue name (args[0]->ToString ());
	
	SANE_Handle handle;
	SANE_Status status;
	status = sane_open (*name, &handle);
	
	Local<ObjectTemplate> result = ObjectTemplate::New ();
	Local<Object> object = result->NewInstance ();
	object->Set (String::New ("status"), Integer::New (status));
	if (status == SANE_STATUS_GOOD) {
		object->Set (String::New ("handle"), SaneHandle::Wrap (handle));
	}

	return scope.Close (object);
}

struct CloseBaton {
	uv_work_t request;
	Persistent<Function> callback;
	SANE_Handle handle;
};

void CloseAsyncWork (uv_work_t* req) {
	CloseBaton* baton = static_cast<CloseBaton*> (req->data);
	sane_close (baton->handle);
}

void CloseAsyncAfter (uv_work_t* req) {
	HandleScope scope;
	CloseBaton* baton = static_cast<CloseBaton*> (req->data);

	Handle<Value> argv[] = { };
	baton->callback->Call (Context::GetCurrent ()->Global(),
		0, argv);

	baton->callback.Dispose ();
	delete[] baton;
}

Handle<Value>
Close (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 2) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 2 arguments")));
	}

	if (!args[0]->IsObject ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an object")));
	}

	if (!args[1]->IsFunction ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be a callback function")));
	}

	Local<Object> handle = args[0]->ToObject ();
	Local<Function> callback = Local<Function>::Cast (args[1]);
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);

	CloseBaton* baton = new CloseBaton ();
	baton->request.data = baton;
	baton->callback = Persistent<Function>::New (callback);
	baton->handle = ptr;

	uv_queue_work (uv_default_loop (), &baton->request,
		CloseAsyncWork, (uv_after_work_cb) CloseAsyncAfter);

	return scope.Close (Undefined ());
}

Handle<Value>
CloseSync (const Arguments& args) {
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
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);
	
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
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);
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
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);
	Local<Integer> n = args[1]->ToInteger ();
	Local<Integer> a = args[2]->ToInteger ();
	char* v = Buffer::Data (args[3]->ToObject ());

	SANE_Status status;
	SANE_Int i;
	status = sane_control_option (ptr, n->Value (), (SANE_Action) a->Value (), v, &i);

	// Perform byte order conversion.
	size_t v_words = Buffer::Length (args[3]->ToObject ()) / sizeof (uint32_t);
	for (size_t j = 0; j < v_words; j++) {
		uint32_t* v_uint32 = (uint32_t*) v;
		v_uint32[j] = htonl (v_uint32[j]);
	}

	return scope.Close (Number::New (status));
}

Handle<Value>
OptionIsActive (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 1) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 1 argument")));
	}

	if (!args[0]->IsInt32 ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an integer")));
	}

	Local<Integer> cap = args[0]->ToInteger ();
	bool ret = SANE_OPTION_IS_ACTIVE (cap->Value ());

	return scope.Close (Boolean::New (ret));
}

Handle<Value>
OptionIsSettable (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 1) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 1 argument")));
	}

	if (!args[0]->IsInt32 ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an integer")));
	}

	Local<Integer> cap = args[0]->ToInteger ();
	bool ret = SANE_OPTION_IS_SETTABLE (cap->Value ());

	return scope.Close (Boolean::New (ret));
}

Handle<Value>
GetParameters (const Arguments& args) {
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
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);
	SANE_Parameters * parameters = new SANE_Parameters ();

	SANE_Status status;
	status = sane_get_parameters (ptr, parameters);
	if (status == SANE_STATUS_GOOD) {
		return scope.Close (SaneParameters::Wrap (parameters));
	} else {
		return scope.Close (Number::New (status));
	}
}

Handle<Value>
Start (const Arguments& args) {
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
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);

	SANE_Status status;
	status = sane_start (ptr);
	
	return scope.Close (Integer::New (status));
}

struct ReadBaton {
	uv_work_t request;
	Persistent<Function> callback;
	SANE_Handle handle;
	SANE_Byte* buf;
	SANE_Int buf_len;

	// Resultant data
	SANE_Status status;
	SANE_Int len;
};

void ReadAsyncWork (uv_work_t* req) {
	ReadBaton* baton = static_cast<ReadBaton*> (req->data);
	baton->status = sane_read (baton->handle, baton->buf, baton->buf_len,
		&baton->len);
}

void ReadAsyncAfter (uv_work_t* req) {
	HandleScope scope;
	ReadBaton* baton = static_cast<ReadBaton*> (req->data);

	Local<ObjectTemplate> result = ObjectTemplate::New ();
	Local<Object> object = result->NewInstance ();
	object->Set (String::New ("status"), Integer::New (baton->status));

	if (baton->status == SANE_STATUS_GOOD) {
		object->Set (String::New ("length"), Integer::New (baton->len));
	}

	Handle<Value> argv[] = { object };
	baton->callback->Call (Context::GetCurrent ()->Global(),
		1, argv);

	baton->callback.Dispose ();
	delete[] baton;
}

Handle<Value>
Read (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 3) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 3 arguments")));
	}

	if (!args[0]->IsObject ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an object")));
	}

	if (!Buffer::HasInstance (args[1])) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be a buffer")));
	}

	if (!args[2]->IsFunction ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Third argument must be a callback function")));
	}

	Local<Object> handle = args[0]->ToObject ();
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);
	SANE_Byte* buf = (SANE_Byte*) Buffer::Data (args[1]->ToObject ());
	Local<Function> callback = Local<Function>::Cast (args[2]);
	
	ReadBaton* baton = new ReadBaton ();
	baton->request.data = baton;
	baton->callback = Persistent<Function>::New (callback);
	baton->handle = ptr;
	baton->buf = buf;
	baton->buf_len = Buffer::Length (args[1]->ToObject ());

	uv_queue_work (uv_default_loop (), &baton->request,
		ReadAsyncWork, (uv_after_work_cb) ReadAsyncAfter);

	return scope.Close (Undefined ());
}

Handle<Value>
ReadSync (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 2) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 2 arguments")));
	}

	if (!args[0]->IsObject ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an object")));
	}

	if (!Buffer::HasInstance (args[1])) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be a buffer")));
	}

	Local<Object> handle = args[0]->ToObject ();
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);
	SANE_Byte* buf = (SANE_Byte*) Buffer::Data (args[1]->ToObject ());

	SANE_Status status;
	SANE_Int len;
	status = sane_read (ptr, buf, Buffer::Length (args[1]->ToObject ()),
		&len);

	Local<ObjectTemplate> result = ObjectTemplate::New ();
	Local<Object> object = result->NewInstance ();
	object->Set (String::New ("status"), Integer::New (status));
	object->Set (String::New ("length"), Integer::New (len));
	return scope.Close (object);
}

struct CancelBaton {
	uv_work_t request;
	Persistent<Function> callback;
	SANE_Handle handle;
};

void CancelAsyncWork (uv_work_t* req) {
	CancelBaton* baton = static_cast<CancelBaton*> (req->data);
	sane_cancel (baton->handle);
}

void CancelAsyncAfter (uv_work_t* req) {
	HandleScope scope;
	CancelBaton* baton = static_cast<CancelBaton*> (req->data);

	Handle<Value> argv[] = { };
	baton->callback->Call (Context::GetCurrent ()->Global(),
		0, argv);

	baton->callback.Dispose ();
	delete[] baton;
}

Handle<Value>
Cancel (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 2) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 2 arguments")));
	}

	if (!args[0]->IsObject ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an object")));
	}

	if (!args[1]->IsFunction ()) {
		return ThrowException (Exception::TypeError (String::New (
			"Second argument must be a callback function")));
	}

	Local<Object> handle = args[0]->ToObject ();
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);
	Local<Function> callback = Local<Function>::Cast (args[1]);

	CancelBaton* baton = new CancelBaton ();
	baton->request.data = baton;
	baton->callback = Persistent<Function>::New (callback);
	baton->handle = ptr;

	uv_queue_work (uv_default_loop (), &baton->request,
		CancelAsyncWork, (uv_after_work_cb) CancelAsyncAfter);

	return scope.Close (Undefined ());
}

Handle<Value>
CancelSync (const Arguments& args) {
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
	SANE_Handle ptr = ObjectWrap::Unwrap<SANE_Handle*> (handle);

	sane_cancel (ptr);
	
	return scope.Close (Undefined ());
}

Handle<Value>
FixedToNumber (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 1) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 1 argument")));
	}

	if (!args[0]->IsInt32 ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be an integer")));
	}

	Local<Integer> n = args[0]->ToInteger ();
	double d = SANE_UNFIX (n->Value ());

	return scope.Close (Number::New (d));
}

Handle<Value>
NumberToFixed (const Arguments& args) {
	HandleScope scope;

	if (args.Length () < 1) {
		return ThrowException (Exception::TypeError (String::New (
			"There should be exactly 1 argument")));
	}

	if (!args[0]->IsNumber ()) {
		return ThrowException (Exception::TypeError (String::New (
			"First argument must be a number")));
	}

	Local<Number> n = args[0]->ToNumber ();
	int val = SANE_FIX (n->Value ());

	return scope.Close (Integer::New (val));
}

void init (Handle<Object> target) {	
	SaneParameters::Init (target);
	SaneHandle::Init (target);

	SetMethod (target, "init", Init);
	SetMethod (target, "exit", Exit);
	SetMethod (target, "getDevices", GetDevices);
	SetMethod (target, "getDevicesSync", GetDevicesSync);
	SetMethod (target, "open", Open);
	SetMethod (target, "openSync", OpenSync);
	SetMethod (target, "close", Close);
	SetMethod (target, "closeSync", CloseSync);
	SetMethod (target, "getOptionDescriptor", GetOptionDescriptor);
	SetMethod (target, "controlOption", ControlOption);
        SetMethod (target, "optionIsActive", OptionIsActive);
        SetMethod (target, "optionIsSettable", OptionIsSettable);
	SetMethod (target, "getParameters", GetParameters);
	SetMethod (target, "start", Start);
	SetMethod (target, "read", Read);
	SetMethod (target, "readSync", ReadSync); 
	SetMethod (target, "cancel", Cancel);
	SetMethod (target, "cancelSync", CancelSync);
	SetMethod (target, "fixedToNumber", FixedToNumber);
	SetMethod (target, "numberToFixed", NumberToFixed);
}

NODE_MODULE (sane, init)

