#include "util.h"

#include <node.h>

using namespace v8;

void throwMessage(Isolate* isolate, const char* message) {
  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, (message))));
}
