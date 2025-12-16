#pragma once

#include "mirror.hpp"

namespace st {

// Smalltalk-mirror handle for a ByteArray object.
// Note: The indexed bytes are not C++ fields; they live in the object body in object memory.
class ByteArray {
public:
    explicit ByteArray(TaggedValue self) : self_(self) {}
    TaggedValue self() const { return self_; }

private:
    ST_SLOT(self_);
};

} // namespace st
