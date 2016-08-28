#ifndef SPEEX_DECODER_H
#define SPEEX_DECODER_H

#include <node.h>
#include <node_object_wrap.h>

class SpeexDecoder : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  SpeexDecoder();
  ~SpeexDecoder();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void decode(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static int decode(int slot, short **output_frame, char *input_frame, int frame_length);
  static int allocate(int wideband);
  static int throwIfBadSlot(int slot);

  int slot;
};

#endif
