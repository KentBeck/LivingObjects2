#include "tagged_value.hpp"
#include <cstdint>
#include <cstring>

TaggedValue TaggedValue::fromSmallInteger(int32_t n) {
    // Encode: (value << 2) | 0x03
    Value encoded = (static_cast<Value>(static_cast<uint32_t>(n)) << 2) | TAG_INTEGER;
    return TaggedValue(encoded);
}

int32_t TaggedValue::toSmallInteger() const {
    if (!isSmallInteger()) {
        return 0; // Not a SmallInteger
    }
    // Decode: (encoded >> 2) as signed 32-bit
    // Sign-extend if necessary
    int32_t value = static_cast<int32_t>(value_ >> 2);
    return value;
}

TaggedValue TaggedValue::fromPointer(void* ptr) {
    // Pointer must be 4-byte aligned, so bottom 2 bits are 00
    Value ptrValue = reinterpret_cast<Value>(ptr);
    // Ensure alignment
    if (ptrValue & TAG_MASK) {
        // Not aligned - this is an error in real usage
        return TaggedValue(NIL);
    }
    return TaggedValue(ptrValue | TAG_POINTER);
}

void* TaggedValue::toPointer() const {
    if (!isPointer()) {
        return nullptr;
    }
    // Clear tag bits
    Value ptrValue = value_ & ~TAG_MASK;
    return reinterpret_cast<void*>(ptrValue);
}

TaggedValue TaggedValue::nil() {
    return TaggedValue(NIL);
}

TaggedValue TaggedValue::trueValue() {
    return TaggedValue(TRUE);
}

TaggedValue TaggedValue::falseValue() {
    return TaggedValue(FALSE);
}

