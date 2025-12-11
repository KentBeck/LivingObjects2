#include "tagged_value.hpp"
#include <cstdint>
#include <cstring>

TaggedValue TaggedValue::fromSmallInteger(int64_t n) {
    // Encode: (value << 2) | 0x03
    // With 64-bit TaggedValue and 2-bit tag, we have 62 bits for value
    // Mask the value to 62 bits before shifting to prevent overflow
    // 62-bit mask: 0x3FFFFFFFFFFFFFFF (62 bits of 1s)
    constexpr Value VALUE_MASK = 0x3FFFFFFFFFFFFFFFULL;
    Value maskedValue = static_cast<Value>(n) & VALUE_MASK;
    Value encoded = (maskedValue << 2) | TAG_INTEGER;
    return TaggedValue(encoded);
}

int64_t TaggedValue::toSmallInteger() const {
    if (!isSmallInteger()) {
        return 0; // Not a SmallInteger
    }
    // Decode: (encoded >> 2) as signed 64-bit
    // Tag bits are in LOW bits (0-1), value is in bits 2-63
    // Use arithmetic right shift by casting to signed first - this automatically sign-extends
    int64_t signedValue = static_cast<int64_t>(value_);
    int64_t decoded = signedValue >> 2;  // Arithmetic right shift sign-extends
    
    // Mask to ensure we only have 62 bits of value (clear any overflow from sign extension)
    // But wait - arithmetic right shift already handles sign extension correctly
    // The issue might be that we need to ensure the value fits in 62 bits
    // Actually, arithmetic right shift should work correctly for both positive and negative
    return decoded;
}

TaggedValue TaggedValue::fromPointer(void* ptr) {
    // Pointer must be 4-byte aligned, so bottom 2 bits are 00
    // Convert pointer to uint64_t
    uintptr_t ptrValue = reinterpret_cast<uintptr_t>(ptr);
    // Ensure alignment
    if (ptrValue & TAG_MASK) {
        // Not aligned - this is an error in real usage
        return TaggedValue(NIL);
    }
    // Store as 64-bit value
    return TaggedValue(static_cast<Value>(ptrValue) | TAG_POINTER);
}

void* TaggedValue::toPointer() const {
    if (!isPointer()) {
        return nullptr;
    }
    // Clear tag bits and convert back to pointer
    Value ptrValue = value_ & ~TAG_MASK;
    return reinterpret_cast<void*>(static_cast<uintptr_t>(ptrValue));
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

