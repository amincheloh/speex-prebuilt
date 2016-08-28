#include "SpeexDecoder.h"

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

Persistent<Function> SpeexDecoder::constructor;

SpeexDecoder::SpeexDecoder() {};
SpeexDecoder::~SpeexDecoder() {
  if (throwIfBadSlot(slot))
    return;

  speex_bits_destroy(&slots.slots[slot]->bits);
  speex_decoder_destroy(slots.slots[slot]->state);

  free(slots.slots[slot]);
  slots.slots[slot] = 0;
};

void SpeexDecoder::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "SpeexDecoder"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "decode", decode);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "SpeexDecoder"), tpl->GetFunction());
}

void SpeexDecoder::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 1 || !args[0]->IsInt32()) {
    throwMessage(isolate, "Wrong argument");
    return args.GetReturnValue().SetUndefined();
  }

  int wideband = args[0]->NumberValue();

  SpeexDecoder* obj = new SpeexDecoder();
  obj->slot = allocate(wideband);
  obj->Wrap(args.This());

  args.GetReturnValue().Set(args.This());
}

void SpeexDecoder::decode(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();

  SpeexDecoder* obj = ObjectWrap::Unwrap<SpeexDecoder>(args.Holder());
  int slot = obj->slot;
  if (throwIfBadSlot(slot)) {
    return args.GetReturnValue().SetUndefined();
  }

  if (args.Length() != 2) {
    throwMessage(isolate, "Need frame length and data source");
    return args.GetReturnValue().SetUndefined();
  }

  if (!args[0]->IsInt32() || args[0]->Int32Value() < 1) {
    throwMessage(isolate, "Frame length must be positive integer");
    return args.GetReturnValue().SetUndefined();
  }

  if (!Buffer::HasInstance(args[1])) {
    throwMessage(isolate, "Data source must be Buffer");
    return args.GetReturnValue().SetUndefined();
 }

  int frame_length = args[0]->Int32Value();
  Local<Value> buf = args[1];

  char* data = Buffer::Data(buf);
  size_t datalen = Buffer::Length(buf);

  std::vector<char> frame_result;

  short* output_frame = 0;
  char* input_frame = (char*)malloc(sizeof(char)*frame_length);
  int frame_size;

  for (size_t i=0; i<datalen; i+=frame_length) {
    memcpy(input_frame, &data[i], frame_length);
    frame_size = decode(slot, &output_frame, input_frame, frame_length);
    for (int j=0; j<frame_size; ++j) {
      frame_result.push_back(output_frame[j] & 0xff);
      frame_result.push_back((output_frame[j] >> 8) & 0xff);
    }
    free(output_frame);
  }

  free(input_frame);

  MaybeLocal<Object> result = Buffer::Copy(isolate, frame_result.data(), frame_result.size());

  args.GetReturnValue().Set(result.ToLocalChecked());
}

int SpeexDecoder::decode(int slot, short **output_frame, char *input_frame, int frame_length) {
  struct Slot* gob = slots.slots[slot];

  int frame_size; // size after decoded
  speex_decoder_ctl(gob->state, SPEEX_GET_FRAME_SIZE, &frame_size);
  speex_bits_read_from(&gob->bits, input_frame, frame_length);
  *output_frame = (short*)malloc(sizeof(short) * frame_size);
  speex_decode_int(gob->state, &gob->bits, *output_frame);

  return frame_size;
}

int SpeexDecoder::throwIfBadSlot(int slot) {
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

int SpeexDecoder::allocate(int wideband) {
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

  gob->state = speex_decoder_init(mode);

  return slot;
}
