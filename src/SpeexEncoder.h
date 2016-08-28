#ifndef SPEEX_ENCODER_H
#define SPEEX_ENCODER_H

#include <node.h>
#include <node_object_wrap.h>

class SpeexEncoder : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> target);

 private:
  SpeexEncoder();
  ~SpeexEncoder();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void encode(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void getFrameSize(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static int allocate(int wideband, int quality);
  static int throwIfBadSlot(int slot);

  int slot;
};

#endif
