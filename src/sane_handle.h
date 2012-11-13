#ifndef SANE_HANDLE_H
#define SANE_HANDLE_H

#include <sane/sane.h>
#include <node.h>

class SaneHandle : public node::ObjectWrap {
public:
	SaneHandle () { };
	~SaneHandle () { };

	static v8::Handle<v8::Value> New (const v8::Arguments& args);
	static void Init (v8::Handle<v8::Object> target);
	static v8::Handle<v8::Value> Wrap (SANE_Handle handle);

private:
	static v8::Persistent<v8::FunctionTemplate> function_template;
};

#endif /* SANE_HANDLE_H */
