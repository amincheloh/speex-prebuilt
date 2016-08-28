#include <node.h>

#include "SpeexDecoder.h"
#include "SpeexEncoder.h"

using namespace v8;

void InitAll(Local<Object> exports) {
  SpeexEncoder::Init(exports);
  SpeexDecoder::Init(exports);
}

NODE_MODULE(Speex, InitAll)
