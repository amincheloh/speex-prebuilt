#include "SpeexEncoder.h"

#include <node.h>
#include <node_buffer.h>

#include <stdlib.h>
#include <string.h>

#include <vector>

#include "speex/speex.h"
#include "slots.h"

#include "util.h"

using namespace node;
using namespace v8;

static struct SlotVector slots = {
  0,0
};

Persistent<Function> SpeexEncoder::constructor;

int _getFrameSize(int slot);
int _encode(int slot, char **output_frame, short *input_frame);

SpeexEncoder::SpeexEncoder() {};
SpeexEncoder::~SpeexEncoder() {
  if (throwIfBadSlot(slot))
    return;

  speex_bits_destroy(&slots.slots[slot]->bits);
  speex_encoder_destroy(slots.slots[slot]->state);

  free(slots.slots[slot]);
  slots.slots[slot] = 0;
};

void SpeexEncoder::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "SpeexEncoder"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "encode", encode);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getFrameSize", getFrameSize);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "SpeexEncoder"), tpl->GetFunction());
}

void SpeexEncoder::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 2 || !args[0]->IsInt32() || !args[1]->IsInt32()) {
    throwMessage(isolate, "Wrong argument. Require bandwidth and quality");
    return args.GetReturnValue().SetUndefined();
  }

  int wideband = args[0]->NumberValue();
  int quality = args[1]->NumberValue();

  SpeexEncoder* obj = new SpeexEncoder();
  obj->slot = allocate(wideband, quality);
  obj->Wrap(args.This());

  args.GetReturnValue().Set(args.This());
}

void SpeexEncoder::encode(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();

  SpeexEncoder* obj = ObjectWrap::Unwrap<SpeexEncoder>(args.This());
  int slot = obj->slot;
  if (throwIfBadSlot(slot)) {
    return args.GetReturnValue().SetUndefined();
  }

  if (args.Length() != 1) {
    throwMessage(isolate, "Need data source");
    return args.GetReturnValue().SetUndefined();
  }
  if (!Buffer::HasInstance(args[0])) {
    throwMessage(isolate, "Data source must be Buffer");
    return args.GetReturnValue().SetUndefined();
  }

  Local<Value> buf = args[0];

  char* data = Buffer::Data(buf);
  size_t datalen = Buffer::Length(buf);
  size_t pcmLength = datalen/2;

  int frame_size = _getFrameSize(slot);
  if (datalen%(frame_size*sizeof(short)) != 0) {
    throwMessage(isolate, "Invalid buffer size.");
    return args.GetReturnValue().SetUndefined();
  }

  short* pcmData = (short*)malloc(sizeof(short) * pcmLength);
  for (size_t i=0; i<pcmLength; i++) {
    pcmData[i] = (short)(((unsigned char)data[i*2+1]) << 8 |
                         ((unsigned char)data[i*2]));
  }

  std::vector<char> spxResult;

  char *output_frame;
  short* input_frame = (short*)malloc(sizeof(short) * frame_size);

  for (size_t i=0; i<pcmLength; i+=frame_size) {
    memcpy(input_frame, &pcmData[i], sizeof(short) * frame_size);
    int outputLength = _encode(slot, &output_frame, input_frame);
    for(int j=0; j<outputLength; j++) {
      spxResult.push_back(output_frame[j]);
    }
    free(output_frame);
  }
  free(input_frame);
  free(pcmData);

  MaybeLocal<Object> result = Buffer::Copy(isolate, spxResult.data(), spxResult.size());

  args.GetReturnValue().Set(result.ToLocalChecked());
}

void SpeexEncoder::getFrameSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();

  SpeexEncoder* obj = ObjectWrap::Unwrap<SpeexEncoder>(args.This());
  int slot = obj->slot;
  if (throwIfBadSlot(slot))
    return args.GetReturnValue().SetUndefined();

  if (args.Length() != 0) {
    throwMessage(isolate, "No need argument");
    return args.GetReturnValue().SetUndefined();
  }

  args.GetReturnValue().Set(_getFrameSize(slot));
}

int _getFrameSize(int slot) {
  int frame_size;
  struct Slot * gob = slots.slots[slot];
  speex_encoder_ctl(gob->state, SPEEX_GET_FRAME_SIZE, &frame_size);
  return frame_size;
}

int _encode(int slot, char **output_frame, short *input_frame) {
  struct Slot * gob = slots.slots[slot];

  speex_bits_reset(&gob->bits);

  speex_encode_int(gob->state, input_frame, &gob->bits);

  int outputSize = speex_bits_nbytes(&gob->bits);
  *output_frame = (char*)malloc(sizeof(short) * outputSize);

  speex_bits_write(&gob->bits, *output_frame, outputSize);

  return outputSize;
}

int SpeexEncoder::throwIfBadSlot(int slot) {
  Isolate* isolate = Isolate::GetCurrent();
  if (slot >= slots.nslots) {
    throwMessage(isolate, "bogus slot");
    return 1;
  }

  if ((void*)0 == slots.slots[slot]) {
    throwMessage(isolate, "slot is already empty");
    return 1;
  }

  return 0;
}

int SpeexEncoder::allocate(int wideband, int quality) {
  int slot = allocate_slot(&slots);

  slots.slots[slot] = (struct Slot*)malloc(sizeof(struct Slot));

  struct Slot* gob = slots.slots[slot];

  speex_bits_init(&gob->bits);

  const SpeexMode * mode;
  switch (wideband) {
  case 1:
    mode = &speex_wb_mode;
    break;
  case 2:
    mode = &speex_uwb_mode;
    break;
  default:
    mode = &speex_nb_mode;
    break;
 }

  gob->state = speex_encoder_init(mode);
  speex_encoder_ctl(gob->state, SPEEX_SET_QUALITY, &quality);

  return slot;
}
