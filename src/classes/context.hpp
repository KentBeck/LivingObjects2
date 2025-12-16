#pragma once

#include "mirror.hpp"

namespace st {

// Smalltalk-mirror Context (slot-only).
// Note: for now this mirrors only the fields our tests need. More slots will be added as we implement the real Context layout.
class Context {
public:
    Context(TaggedValue method, TaggedValue receiver, TaggedValue stack, TaggedValue instructionPointer)
        : method_(method),
          receiver_(receiver),
          stack_(stack),
          instructionPointer_(instructionPointer) {}

    TaggedValue method() const { return method_; }
    TaggedValue receiver() const { return receiver_; }
    TaggedValue stack() const { return stack_; }
    TaggedValue instructionPointer() const { return instructionPointer_; }

    void setStack(TaggedValue stack) { stack_ = stack; }
    void setInstructionPointer(TaggedValue ip) { instructionPointer_ = ip; }

private:
    ST_SLOT(method_);             // CompiledMethod (object pointer)
    ST_SLOT(receiver_);           // Object
    ST_SLOT(stack_);              // Array (object pointer)
    ST_SLOT(instructionPointer_); // SmallInteger
};

} // namespace st

