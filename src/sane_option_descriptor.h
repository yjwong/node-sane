#ifndef SANE_OPTION_DESCRIPTOR_H
#define SANE_OPTION_DESCRIPTOR_H

#include <sane/sane.h>
#include <node.h>

class SaneOptionDescriptor : public node::ObjectWrap {
public:
	SaneOptionDescriptor (const SANE_Option_Descriptor * descriptor);
	~SaneOptionDescriptor ();

	static v8::Handle<v8::Value> New (const SANE_Option_Descriptor * descriptor);

private:
	const SANE_Option_Descriptor * _descriptor;

	static v8::Handle<v8::Value> GetName (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetTitle (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetDesc (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetType (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetUnit (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetSize (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetCap (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> GetConstraintType (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	//static v8::Handle<v8::Value> GetConstraint (v8::Local<v8::String> property, const v8::AccessorInfo& info);
};

#endif /* SANE_OPTION_DESCRIPTOR_H */
