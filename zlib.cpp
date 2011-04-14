#include <zlib.h>
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <string.h>
#include <iostream>

#include "buffer_compat.h"

using namespace v8;
using namespace node;

Handle<Value> InflateBuffer(const Arguments &args) {
	if (args.Length() != 1) {
		return ThrowException(Exception::TypeError(String::New("zlib.inflateBuffer expects 1 argument")));
	}

	if (!Buffer::HasInstance(args[0])) {
		return ThrowException(Exception::TypeError(String::New("zlib.inflateBuffer expects a Buffer")));
	}

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	if (Z_OK != inflateInit(&strm)) {
		return ThrowException(String::New("inflateInit failed"));		
	}

	const size_t CHUNK = 4096;
	unsigned char out[CHUNK];

	size_t outBufLen = 1024, outBufUsed = 0;
	unsigned char *outBuf = new unsigned char[1024];

	strm.avail_in = BufferLength(args[0]->ToObject());
	strm.next_in = (unsigned char*) BufferData(args[0]->ToObject());

	do {
		strm.avail_out = CHUNK;
		strm.next_out = out;

		if (Z_STREAM_END != inflate(&strm, Z_NO_FLUSH)) {
			inflateEnd(&strm);
			delete[] outBuf;
			return ThrowException(String::New("inflate failed"));
		}

		size_t have = CHUNK - strm.avail_out;

		if (have + outBufUsed > outBufLen) {
			outBufLen *= 2;

			unsigned char *newBuf = new unsigned char[1024];
			memcpy(newBuf, outBuf, outBufUsed);

			delete[] outBuf;

			outBuf = newBuf;
		}

		memcpy(outBuf + outBufUsed, out, have);
		outBufUsed += have;
	}
	while (strm.avail_out == 0);

	inflateEnd(&strm);

	return Buffer::New((char*)outBuf, outBufUsed)->handle_;
}

Handle<Value> DeflateBuffer(const Arguments &args) {
	return Undefined();
}

extern "C" void init(Handle<Object> target) {
	HandleScope scope;
	Local<FunctionTemplate> t;
	
	t = FunctionTemplate::New(&InflateBuffer);
	target->Set(String::NewSymbol("inflateBuffer"), t->GetFunction());

	t = FunctionTemplate::New(&DeflateBuffer);
	target->Set(String::NewSymbol("deflateBuffer"), t->GetFunction());
}
