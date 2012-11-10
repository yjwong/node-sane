#ifndef SANE_HANDLE_H
#define SANE_HANDLE_H

#include <sane/sane.h>
#include <node.h>

class SaneHandle : public node::ObjectWrap {
public:
	SaneHandle (SANE_Handle device);
	~SaneHandle ();

	static v8::Handle<v8::Value> New (SANE_Handle handle);

private:
	SANE_Handle _handle;
};

#endif /* SANE_HANDLE_H */
