#ifndef SANE_PARAMETERS_H
#define SANE_PARAMETERS_H

#include <sane/sane.h>
#include <node.h>

class SaneParameters : public node::ObjectWrap {
public:
	SaneParameters () { };
	~SaneParameters () { };

	static void Init (v8::Handle<v8::Object> target);
	static v8::Handle<v8::Value> New (const v8::Arguments& args);
	static v8::Handle<v8::Value> Wrap (SANE_Parameters * parameters);
	using node::ObjectWrap::Wrap;

private:
	static v8::Persistent<v8::FunctionTemplate> constructor_template;
	SANE_Parameters * _parameters;

	static v8::Handle<v8::Value> GetFormat (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void SetFormat (v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	static v8::Handle<v8::Value> GetLastFrame (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void SetLastFrame (v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	static v8::Handle<v8::Value> GetBytesPerLine (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void SetBytesPerLine (v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	static v8::Handle<v8::Value> GetPixelsPerLine (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void SetPixelsPerLine (v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	static v8::Handle<v8::Value> GetLines (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void SetLines (v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	static v8::Handle<v8::Value> GetDepth (v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static void SetDepth (v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
};

#endif /* SANE_PARAMETERS_H */
