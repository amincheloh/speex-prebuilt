#include "speex/speex.h"

struct Slot {
  SpeexBits	bits;
  void *state;
};

struct SlotVector
{
  struct Slot **slots;
  int nslots;
};

#ifdef __cplusplus
extern "C" {
#endif

int allocate_slot(struct SlotVector*slots_ptr);

#ifdef __cplusplus
}
#endif
