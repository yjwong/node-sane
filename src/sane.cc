#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <node.h>
#include <node_buffer.h>
#include <sane/sane.h>
#include <v8.h>
#include "sane_device.h"
#include "sane_handle.h"
#include "sane_option_descriptor.h"
#include "sane_parameters.h"

using v8::Number;
using v8::Local;
using v8::Object;
using v8::ObjectTemplate;
using v8::String;
using v8::Integer;
using v8::Array;
using v8::Value;
using v8::Boolean;
using v8::Function;
using v8::Handle;
using v8::External;
using v8::FunctionTemplate;

// These things need to be moved into a class, so each instance can have its own session.
// Right now, there can only be one SANE instance at a time.
SANE_Int version_code;

NAN_METHOD(Init) {
    Nan::HandleScope scope;
    SANE_Status status;
    status = sane_init(&version_code, NULL); // TODO: Set authorization callback.
    info.GetReturnValue().Set(Nan::New(status));
}

NAN_METHOD(Exit) {
    Nan::HandleScope scope;
    sane_exit();
    info.GetReturnValue().Set(Nan::Undefined());
}

class GetDevicesWorker : public Nan::AsyncWorker {
public:
    GetDevicesWorker(Nan::Callback* callback, bool localOnly) :
        Nan::AsyncWorker(callback), localOnly(localOnly) {};
    ~GetDevicesWorker() {};

    void Execute() {
        status = sane_get_devices(&deviceList, localOnly);
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;

        Local<ObjectTemplate> result = Nan::New<ObjectTemplate>();
        Local<Object> object = result->NewInstance();
        object->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));

        if (status == SANE_STATUS_GOOD) {
            Local<Array> deviceListArr = Nan::New<Array>();
            for (int i = 0; deviceList[i]; i++) {
                Local<FunctionTemplate> deviceTpl = Nan::New(SaneDevice::constructor_template);
                Local<Function> deviceFunc = deviceTpl->GetFunction();
                Handle<Value> deviceFuncArgs[] = { Nan::New<External>(const_cast<SANE_Device*>(deviceList[i])) };
                Local<Value> deviceObj = deviceFunc->NewInstance(1, deviceFuncArgs);
                deviceListArr->Set(i, deviceObj);
            }
            object->Set(Nan::New("deviceList").ToLocalChecked(), deviceListArr);
        }

        Local<Value> argv[] = { object };
        callback->Call(1, argv);
    }

private:
    bool localOnly;
    SANE_Status status;
    const SANE_Device** deviceList;
};

NAN_METHOD(GetDevices) {
    Nan::HandleScope scope;

    if (info.Length() < 2) {
        return Nan::ThrowTypeError("There should be exactly 2 arguments");
    }

    if (!info[0]->IsBoolean()) {
        return Nan::ThrowTypeError("First argument must be a boolean");
    }

    if (!info[1]->IsFunction()) {
        return Nan::ThrowTypeError("Second argument must be a callback function");
    }

    bool localOnly = Nan::To<Boolean>(info[0]).ToLocalChecked()->Value();
    Nan::Callback* callback = new Nan::Callback(info[1].As<Function>());
    Nan::AsyncQueueWorker(new GetDevicesWorker(callback, localOnly));
}

NAN_METHOD(GetDevicesSync) {
    Nan::HandleScope scope;

    if (info.Length() < 1) {
        return Nan::ThrowTypeError("There should be exactly 1 argument");
    }

    if (!info[0]->IsBoolean()) {
        return Nan::ThrowTypeError("First argument must be a boolean");
    }

    bool localOnly = Nan::To<Boolean>(info[0]).ToLocalChecked()->Value();
    
    const SANE_Device** deviceList;
    SANE_Status status;
    status = sane_get_devices(&deviceList, localOnly);

    Local<ObjectTemplate> result = Nan::New<ObjectTemplate>();
    Local<Object> object = result->NewInstance();
    object->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));

    if (status == SANE_STATUS_GOOD) {
        Local<Array> deviceListArr = Nan::New<Array>();
        for (int i = 0; deviceList[i]; i++) {
            Local<FunctionTemplate> deviceTpl = Nan::New(SaneDevice::constructor_template);
            Local<Function> deviceFunc = deviceTpl->GetFunction();
            Handle<Value> deviceFuncArgs[] = { Nan::New<External>(const_cast<SANE_Device*>(deviceList[i])) };
            Local<Value> deviceObj = deviceFunc->NewInstance(1, deviceFuncArgs);
            deviceListArr->Set(i, deviceObj);
        }
        object->Set(Nan::New("deviceList").ToLocalChecked(), deviceListArr);
    }

    info.GetReturnValue().Set(object);
}

class OpenWorker : public Nan::AsyncWorker {
public:
    OpenWorker(Nan::Callback* callback, SANE_String_Const name) :
        Nan::AsyncWorker(callback), name(name) {};
    ~OpenWorker() {};

    void Execute() {
        status = sane_open(name, &handle);
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;

        Local<ObjectTemplate> result = Nan::New<ObjectTemplate>();
        Local<Object> object = result->NewInstance();
        object->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));
        if (status == SANE_STATUS_GOOD) {
            Local<FunctionTemplate> handleTpl = Nan::New(SaneHandle::constructor_template);
            Local<Function> handleFunc = handleTpl->GetFunction();
            Handle<Value> handleFuncArgs[] = { Nan::New<External>(handle) };
            Local<Value> handleObj = handleFunc->NewInstance(1, handleFuncArgs);
            object->Set(Nan::New("handle").ToLocalChecked(), handleObj);
        }

        Local<Value> argv[] = { object };
        callback->Call(1, argv);
    }

private:
    SANE_String_Const name;
    SANE_Handle handle;
    SANE_Status status;
};

NAN_METHOD(Open) {
    Nan::HandleScope scope;

    if (info.Length() < 2) {
        return Nan::ThrowTypeError("There should be exactly 2 arguments");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    if (!info[1]->IsFunction()) {
        return Nan::ThrowTypeError("Second argument must be a callback function");
    }

    String::Utf8Value nameArg (info[0].As<String>());
    std::string name (*nameArg);
    Nan::Callback* callback = new Nan::Callback(info[1].As<Function>());
    Nan::AsyncQueueWorker(new OpenWorker(callback, name.c_str()));
}

NAN_METHOD(OpenSync) {
    Nan::HandleScope scope;

    if (info.Length() < 1) {
        return Nan::ThrowTypeError("There should be exactly 1 argument");
    }

    if (!info[0]->IsString()) {
        return Nan::ThrowTypeError("First argument must be a string");
    }

    String::Utf8Value nameArg (info[0].As<String>());
    std::string name (*nameArg);
    
    SANE_Handle handle;
    SANE_Status status;
    status = sane_open(name.c_str(), &handle);

    Local<ObjectTemplate> result = Nan::New<ObjectTemplate>();
    Local<Object> object = result->NewInstance();
    object->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));

    if (status == SANE_STATUS_GOOD) {
        Local<FunctionTemplate> handleTpl = Nan::New(SaneHandle::constructor_template);
        Local<Function> handleFunc = handleTpl->GetFunction();
        Handle<Value> handleFuncArgs[] = { Nan::New<External>(handle) };
        Local<Value> handleObj = handleFunc->NewInstance(1, handleFuncArgs);
        object->Set(Nan::New("handle").ToLocalChecked(), handleObj);
    }

    info.GetReturnValue().Set(object);
}

class CloseWorker : public Nan::AsyncWorker {
public:
    CloseWorker(Nan::Callback* callback, SANE_Handle handle) :
        Nan::AsyncWorker(callback), handle(handle) {};
    ~CloseWorker() {};

    void Execute() {
        sane_close(&handle);
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;
        Local<Value> argv[] = {};
        callback->Call(0, argv);
    }
    
private:
    SANE_Handle handle;
};

NAN_METHOD(Close) {
    Nan::HandleScope scope;

    if (info.Length() < 2) {
        return Nan::ThrowTypeError("There should be exactly 2 arguments");
    }

    if (!info[0]->IsObject()) {
        return Nan::ThrowTypeError("First argument must be an object");
    }

    if (!info[1]->IsFunction()) {
        return Nan::ThrowTypeError("Second argument must be a callback function");
    }

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
    SANE_Handle ptr = handle->getHandle();

    Nan::Callback* callback = new Nan::Callback(info[1].As<Function>());
    Nan::AsyncQueueWorker(new CloseWorker(callback, ptr));
}

NAN_METHOD(CloseSync) {
    Nan::HandleScope scope;

    if (info.Length() < 1) {
        return Nan::ThrowTypeError("There should be exactly 1 argument");
    }

    if (!info[0]->IsObject()) {
        return Nan::ThrowTypeError("First argument must be an object");
    }

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
    SANE_Handle ptr = handle->getHandle();

    sane_close(ptr);
    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetOptionDescriptor) {
    Nan::HandleScope scope;

	if (info.Length() < 2) {
		return Nan::ThrowTypeError("There should be exactly 2 arguments");
	}

	if (!info[0]->IsObject()) {
        return Nan::ThrowTypeError("First argument must be an object");
	}

	if (!info[1]->IsInt32()) {
        return Nan::ThrowTypeError("Second argument must be an integer");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
	Local<Integer> n = info[1].As<Integer>();
	
	const SANE_Option_Descriptor* option = sane_get_option_descriptor(
		handle->getHandle(), n->Value());
	if (option == NULL) {
        return info.GetReturnValue().Set(Nan::Undefined());
	}

    Local<FunctionTemplate> descriptorTpl = Nan::New(SaneOptionDescriptor::constructor_template);
    Local<Function> descriptorFunc = descriptorTpl->GetFunction();
    Handle<Value> descriptorFuncArgs[] = { Nan::New<External>(const_cast<SANE_Option_Descriptor*>(option)) };
    Local<Value> descriptorObj = descriptorFunc->NewInstance(1, descriptorFuncArgs);
    
    info.GetReturnValue().Set(descriptorObj);
}

NAN_METHOD(ControlOption) {
    Nan::HandleScope scope;

	if (info.Length() < 4) {
        return Nan::ThrowTypeError("There should be exactly 5 arguments");
	}

	if (!info[0]->IsObject()) {
        return Nan::ThrowTypeError("First argument must be an object");
	}

	if (!info[1]->IsInt32()) {
        return Nan::ThrowTypeError("Second argument must be an integer");
	}

	if (!info[2]->IsInt32()) {
        return Nan::ThrowTypeError("Third argument must be an integer");
	}

	if (!node::Buffer::HasInstance(info[3])) {
        return Nan::ThrowTypeError("Fourth argument must be a buffer");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
	Local<Integer> n = info[1].As<Integer>();
	Local<Integer> a = info[2].As<Integer>();
	char* v = node::Buffer::Data(info[3].As<Object>());

	SANE_Status status;
	SANE_Int i;
	status = sane_control_option(handle->getHandle(), n->Value(), (SANE_Action) a->Value(), v, &i);

	// Perform byte order conversion.
	size_t v_words = node::Buffer::Length (info[3]->ToObject()) / sizeof (uint32_t);
	for (size_t j = 0; j < v_words; j++) {
		uint32_t* v_uint32 = (uint32_t*) v;
		v_uint32[j] = htonl(v_uint32[j]);
	}

    info.GetReturnValue().Set(Nan::New(status));
}

NAN_METHOD(OptionIsActive) {
    Nan::HandleScope scope;

	if (info.Length() < 1) {
        return Nan::ThrowTypeError("There should be exactly 1 argument");
	}

	if (!info[0]->IsInt32()) {
        return Nan::ThrowTypeError("First argument must be an integer");
	}

	Local<Integer> cap = info[0].As<Integer>();
	bool ret = SANE_OPTION_IS_ACTIVE(cap->Value());

    info.GetReturnValue().Set(Nan::New(ret));
}

NAN_METHOD(OptionIsSettable) {
    Nan::HandleScope scope;

	if (info.Length() < 1) {
	    return Nan::ThrowTypeError("There should be exactly 1 argument");
	}

	if (!info[0]->IsInt32()) {
		return Nan::ThrowTypeError("First argument must be an integer");
	}

	Local<Integer> cap = info[0].As<Integer>();
	bool ret = SANE_OPTION_IS_SETTABLE(cap->Value());

    info.GetReturnValue().Set(Nan::New(ret));
}

NAN_METHOD(GetParameters) {
    Nan::HandleScope scope;

	if (info.Length() < 1) {
		return Nan::ThrowTypeError("There should be exactly 1 argument");
	}

	if (!info[0]->IsObject()) {
        return Nan::ThrowTypeError("First argument must be an object");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
	SANE_Parameters *parameters = new SANE_Parameters();

	SANE_Status status;
	status = sane_get_parameters(handle->getHandle(), parameters);
	if (status == SANE_STATUS_GOOD) {
        Local<FunctionTemplate> paramsTpl = Nan::New(SaneParameters::constructor_template);
        Local<Function> paramsFunc = paramsTpl->GetFunction();
        Handle<Value> paramsFuncArgs[] = { Nan::New<External>(parameters) };
        Local<Value> paramsObj = paramsFunc->NewInstance(1, paramsFuncArgs);
        info.GetReturnValue().Set(paramsObj);
	} else {
        info.GetReturnValue().Set(Nan::New(status));
	}
}

NAN_METHOD(Start) {
    Nan::HandleScope scope;

	if (info.Length() < 1) {
		return Nan::ThrowTypeError("There should be exactly 1 argument");
	}

	if (!info[0]->IsObject()) {
		return Nan::ThrowTypeError("First argument must be an object");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);

	SANE_Status status;
	status = sane_start(handle->getHandle());

    info.GetReturnValue().Set(Nan::New(status));
}

class ReadWorker : public Nan::AsyncWorker {
public:
    ReadWorker(Nan::Callback* callback, SANE_Handle handle, SANE_Byte* buf,
            SANE_Int bufLen) :
        Nan::AsyncWorker(callback), handle(handle), buf(buf), bufLen(bufLen) {};
    ~ReadWorker() {};

    void Execute() {
        status = sane_read(handle, buf, bufLen, &len);
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;

        Local<ObjectTemplate> result = Nan::New<ObjectTemplate>();
        Local<Object> object = result->NewInstance();
        object->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));

        if (status == SANE_STATUS_GOOD) {
            object->Set(Nan::New("length").ToLocalChecked(), Nan::New(len));
        }

        Local<Value> argv[] = { object };
        callback->Call(1, argv);
    }

private:
    SANE_Handle handle;
    SANE_Byte* buf;
    SANE_Int bufLen;

    SANE_Status status;
    SANE_Int len;
};

NAN_METHOD(Read) {
    Nan::HandleScope scope;

	if (info.Length() < 3) {
		return Nan::ThrowTypeError("There should be exactly 3 arguments");
	}

	if (!info[0]->IsObject()) {
		return Nan::ThrowTypeError("First argument must be an object");
	}

	if (!node::Buffer::HasInstance(info[1])) {
		return Nan::ThrowTypeError("Second argument must be a buffer");
	}

	if (!info[2]->IsFunction()) {
        return Nan::ThrowTypeError("Third argument must be a callback function");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
	SANE_Byte* buf = (SANE_Byte*) node::Buffer::Data(info[1].As<Object>());
    Nan::Callback* callback = new Nan::Callback(info[2].As<Function>());
    Nan::AsyncQueueWorker(new ReadWorker(callback, handle->getHandle(), buf, node::Buffer::Length(info[1].As<Object>())));    
}

NAN_METHOD(ReadSync) {
    Nan::HandleScope scope;

	if (info.Length() < 2) {
		return Nan::ThrowTypeError("There should be exactly 2 arguments");
	}

	if (!info[0]->IsObject()) {
		return Nan::ThrowTypeError("First argument must be an object");
	}

	if (!node::Buffer::HasInstance(info[1])) {
		return Nan::ThrowTypeError("Second argument must be a buffer");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
	SANE_Byte* buf = (SANE_Byte*) node::Buffer::Data(info[1].As<Object>());

	SANE_Status status;
	SANE_Int len;
	status = sane_read(handle->getHandle(), buf, node::Buffer::Length(info[1].As<Object>()),
		&len);

	Local<ObjectTemplate> result = ObjectTemplate::New();
	Local<Object> object = result->NewInstance();
	object->Set(Nan::New("status").ToLocalChecked(), Nan::New(status));
	object->Set(Nan::New("length").ToLocalChecked(), Nan::New(len));
    info.GetReturnValue().Set(object);
}

class CancelWorker : public Nan::AsyncWorker {
public:
    CancelWorker(Nan::Callback *callback, SANE_Handle* handle) :
        Nan::AsyncWorker(callback), handle(handle) {};
    ~CancelWorker() {};

    void Execute() {
        sane_cancel(&handle);
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;
        Local<Value> argv[] = {};
        callback->Call(0, argv);
    }

private:
    SANE_Handle* handle;
};

NAN_METHOD(Cancel) {
    Nan::HandleScope scope;

	if (info.Length() < 2) {
		return Nan::ThrowTypeError("There should be exactly 2 arguments");
	}

	if (!info[0]->IsObject()) {
		return Nan::ThrowTypeError("First argument must be an object");
	}

	if (!info[1]->IsFunction()) {
		return Nan::ThrowTypeError("Second argument must be a callback function");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);
    Nan::Callback* callback = new Nan::Callback(info[1].As<Function>());
    Nan::AsyncQueueWorker(new CloseWorker(callback, handle->getHandle()));
}

NAN_METHOD(CancelSync) {
    Nan::HandleScope scope;

	if (info.Length() < 1) {
		return Nan::ThrowTypeError("There should be exactly 1 argument");
	}

	if (!info[0]->IsObject()) {
		return Nan::ThrowTypeError("First argument must be an object");
	}

    Local<Object> handleObj = Nan::To<Object>(info[0]).ToLocalChecked();
    SaneHandle* handle = Nan::ObjectWrap::Unwrap<SaneHandle>(handleObj);

	sane_cancel(handle->getHandle());
	info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(FixedToNumber) {
    Nan::HandleScope scope;

	if (info.Length() < 1) {
		return Nan::ThrowTypeError("There should be exactly 1 argument");
	}

	if (!info[0]->IsInt32()) {
		return Nan::ThrowTypeError("First argument must be an integer");
	}

	Local<Integer> n = info[0].As<Integer>();
	double d = SANE_UNFIX(n->Value());

    info.GetReturnValue().Set(Nan::New(d));
}

NAN_METHOD(NumberToFixed) {
    Nan::HandleScope scope;

	if (info.Length() < 1) {
		return Nan::ThrowTypeError("There should be exactly 1 argument");
	}

	if (!info[0]->IsNumber()) {
		return Nan::ThrowTypeError("First argument must be a number");
	}

	Local<Number> n = info[0].As<Number>();
	int val = SANE_FIX(n->Value());

    info.GetReturnValue().Set(Nan::New(val));
}

void InitAll(Handle<Object> exports) {
    SaneParameters::Init(exports);
    SaneHandle::Init(exports);
    SaneDevice::Init(exports);
    SaneOptionDescriptor::Init(exports);

    Nan::SetMethod(exports, "init", Init);
    Nan::SetMethod(exports, "exit", Exit);
    Nan::SetMethod(exports, "getDevices", GetDevices);
    Nan::SetMethod(exports, "getDevicesSync", GetDevicesSync);
    Nan::SetMethod(exports, "open", Open);
    Nan::SetMethod(exports, "openSync", OpenSync);
    Nan::SetMethod(exports, "close", Close);
    Nan::SetMethod(exports, "closeSync", CloseSync);
    Nan::SetMethod(exports, "getOptionDescriptor", GetOptionDescriptor);
    Nan::SetMethod(exports, "controlOption", ControlOption);
    Nan::SetMethod(exports, "optionIsActive", OptionIsActive);
    Nan::SetMethod(exports, "optionIsSettable", OptionIsSettable);
    Nan::SetMethod(exports, "getParameters", GetParameters);
    Nan::SetMethod(exports, "start", Start);
    Nan::SetMethod(exports, "read", Read);
    Nan::SetMethod(exports, "readSync", ReadSync);
    Nan::SetMethod(exports, "cancel", Cancel);
    Nan::SetMethod(exports, "cancelSync", CancelSync);
    Nan::SetMethod(exports, "fixedToNumber", FixedToNumber);
    Nan::SetMethod(exports, "numberToFixed", NumberToFixed);
}

NODE_MODULE(sane, InitAll)

