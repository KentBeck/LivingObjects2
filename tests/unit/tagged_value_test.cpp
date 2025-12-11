#include "../src/tagged_value.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <cstddef>

// ============================================================================
// Size and Layout Tests
// ============================================================================

TEST(TaggedValue, SizeIs64Bits) {
    // TaggedValue must be exactly 64 bits (8 bytes)
    ASSERT_EQ(sizeof(TaggedValue), 8) << "TaggedValue must be 64 bits (8 bytes)";
    ASSERT_EQ(sizeof(TaggedValue::Value), 8) << "TaggedValue::Value must be 64 bits";
}

TEST(TaggedValue, ValueTypeIsUint64) {
    // Verify the underlying type is uint64_t
    static_assert(std::is_same_v<TaggedValue::Value, uint64_t>, 
                  "TaggedValue::Value must be uint64_t");
}

// ============================================================================
// SmallInteger Tests
// ============================================================================

TEST(TaggedValue, SmallIntegerEncoding) {
    TaggedValue zero = TaggedValue::fromSmallInteger(0);
    ASSERT_TRUE(zero.isSmallInteger());
    ASSERT_EQ(zero.toSmallInteger(), 0);
    
    TaggedValue one = TaggedValue::fromSmallInteger(1);
    ASSERT_TRUE(one.isSmallInteger());
    ASSERT_EQ(one.toSmallInteger(), 1);
    
    TaggedValue negative = TaggedValue::fromSmallInteger(-1);
    ASSERT_TRUE(negative.isSmallInteger());
    ASSERT_EQ(negative.toSmallInteger(), -1);
    
    // Test 62-bit signed integer range (64-bit TaggedValue with 2-bit tag = 62 bits for value)
    // Maximum: 2^61 - 1 = 0x1FFFFFFFFFFFFFFF (bits 0-60 are 1, bit 61 is 0)
    // Minimum: -2^61 = -0x2000000000000000 (bit 61 is 1, sign bit)
    TaggedValue max = TaggedValue::fromSmallInteger(0x1FFFFFFFFFFFFFFFLL);
    ASSERT_TRUE(max.isSmallInteger());
    ASSERT_EQ(max.toSmallInteger(), 0x1FFFFFFFFFFFFFFFLL);
    
    TaggedValue min = TaggedValue::fromSmallInteger(-0x2000000000000000LL);
    ASSERT_TRUE(min.isSmallInteger());
    ASSERT_EQ(min.toSmallInteger(), -0x2000000000000000LL);
}

// ============================================================================
// Special Values Tests
// ============================================================================

TEST(TaggedValue, SpecialValues) {
    TaggedValue nil = TaggedValue::nil();
    ASSERT_TRUE(nil.isNil());
    ASSERT_TRUE(nil.isSpecial());
    ASSERT_FALSE(nil.isSmallInteger());
    ASSERT_FALSE(nil.isPointer());
    
    TaggedValue trueVal = TaggedValue::trueValue();
    ASSERT_TRUE(trueVal.isTrue());
    ASSERT_TRUE(trueVal.isBoolean());
    ASSERT_TRUE(trueVal.isSpecial());
    
    TaggedValue falseVal = TaggedValue::falseValue();
    ASSERT_TRUE(falseVal.isFalse());
    ASSERT_TRUE(falseVal.isBoolean());
    ASSERT_TRUE(falseVal.isSpecial());
}

// ============================================================================
// Pointer Tests
// ============================================================================

TEST(TaggedValue, PointerEncoding) {
    // Create a test pointer (aligned to 4 bytes)
    void* testPtr = reinterpret_cast<void*>(0x12345678);
    TaggedValue ptr = TaggedValue::fromPointer(testPtr);
    
    ASSERT_TRUE(ptr.isPointer());
    ASSERT_FALSE(ptr.isSmallInteger());
    ASSERT_FALSE(ptr.isSpecial());
    
    void* decoded = ptr.toPointer();
    ASSERT_EQ(decoded, testPtr);
}

TEST(TaggedValue, PointerEncoding64Bit) {
    // Test with a 64-bit pointer value (high bits set)
    // This ensures TaggedValue correctly handles 64-bit pointers
    void* testPtr64 = reinterpret_cast<void*>(0x123456789ABCDEF0ULL);
    TaggedValue ptr64 = TaggedValue::fromPointer(testPtr64);
    
    ASSERT_TRUE(ptr64.isPointer());
    
    void* decoded64 = ptr64.toPointer();
    ASSERT_EQ(decoded64, testPtr64) << "64-bit pointer must be preserved correctly";
    
    // Verify the value is stored as 64 bits
    TaggedValue::Value storedValue = ptr64.value();
    TaggedValue::Value expectedValue = reinterpret_cast<uintptr_t>(testPtr64) | TaggedValue::TAG_POINTER;
    ASSERT_EQ(storedValue, expectedValue) << "Pointer value must be stored as 64-bit";
}

// ============================================================================
// Test Runner Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

