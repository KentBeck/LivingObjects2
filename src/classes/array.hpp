#pragma once

#include "mirror.hpp"

namespace st {

// Smalltalk-mirror handle for an Array object.
// Note: The indexed elements are not C++ fields; they live in the object body in object memory.
class Array {
public:
    explicit Array(TaggedValue self) : self_(self) {}
    TaggedValue self() const { return self_; }

private:
    ST_SLOT(self_);
};

} // namespace st
