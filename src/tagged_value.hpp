#pragma once

#include <cstdint>
#include <cstddef>

/**
 * TaggedValue - Tagged value representation for Smalltalk VM
 * 
 * Uses 2-bit tagging scheme:
 * - 00: Pointer (heap-allocated object)
 * - 01: Special (nil, true, false)
 * - 10: Float (simplified, limited support)
 * - 11: SmallInteger (31-bit signed integer)
 */
class TaggedValue {
public:
    using Value = uintptr_t;
    
    // Tag constants
    static constexpr Value TAG_POINTER = 0x00;
    static constexpr Value TAG_SPECIAL = 0x01;
    static constexpr Value TAG_FLOAT = 0x02;
    static constexpr Value TAG_INTEGER = 0x03;
    static constexpr Value TAG_MASK = 0x03;
    
    // Special values
    static constexpr Value NIL = 0x00000001;
    static constexpr Value TRUE = 0x00000005;
    static constexpr Value FALSE = 0x00000009;
    
    // Construction
    TaggedValue() : value_(NIL) {}
    explicit TaggedValue(Value value) : value_(value) {}
    
    // Type checking
    bool isPointer() const { return (value_ & TAG_MASK) == TAG_POINTER; }
    bool isSmallInteger() const { return (value_ & TAG_MASK) == TAG_INTEGER; }
    bool isSpecial() const { return (value_ & TAG_MASK) == TAG_SPECIAL; }
    bool isNil() const { return value_ == NIL; }
    bool isTrue() const { return value_ == TRUE; }
    bool isFalse() const { return value_ == FALSE; }
    bool isBoolean() const { return isTrue() || isFalse(); }
    
    // Encoding/decoding
    static TaggedValue fromSmallInteger(int32_t n);
    int32_t toSmallInteger() const;
    
    static TaggedValue fromPointer(void* ptr);
    void* toPointer() const;
    
    // Special values
    static TaggedValue nil();
    static TaggedValue trueValue();
    static TaggedValue falseValue();
    
    // Access
    Value value() const { return value_; }
    
    // Comparison
    bool operator==(const TaggedValue& other) const { return value_ == other.value_; }
    bool operator!=(const TaggedValue& other) const { return value_ != other.value_; }
    
private:
    Value value_;
};

