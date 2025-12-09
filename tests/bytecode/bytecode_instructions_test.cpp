#include "bytecode_test_helpers.hpp"
#include "vm_test_types.hpp"  // For CompiledMethod and Context (stubs until Phase 2.5/3.1)
#include "../src/tagged_value.hpp"  // Real TaggedValue implementation
#include <gtest/gtest.h>
#include <vector>
#include <cstdint>

using namespace test_helpers;

/**
 * Test Pattern:
 * 1. Create a CompiledMethod in memory with bytecode
 * 2. Create a Context for executing the method
 * 3. Step forward one instruction
 * 4. Check the post conditions
 */

// ============================================================================
// PUSH_LITERAL (opcode 0)
// ============================================================================

/**
 * Test PUSH_LITERAL instruction
 * 
 * Format: 00 [index:uint32_le]
 * Stack: ... → ..., literal
 * 
 * Post conditions:
 * - Stack depth increased by 1
 * - Top of stack equals literals[index]
 * - Instruction pointer advanced by 5
 */
TEST(BytecodeInstructions, PushLiteral_Basic) {
    // Arrange: Create bytecode for PUSH_LITERAL with index 0
    std::vector<uint8_t> bytecode = encodeInstruction(0, {0}); // PUSH_LITERAL index 0
    
    // Create literals array
    std::vector<TaggedValue> literals = {
        makeSmallInteger(42)
    };
    
    // Create CompiledMethod
    auto method = createCompiledMethod(bytecode, literals);
    
    // Create Context with receiver (self)
    TaggedValue receiver = makeSmallInteger(0); // self
    auto context = createContext(method.get(), receiver);
    
    // Pre-conditions
    ASSERT_EQ(getStackDepth(context.get()), 0);
    ASSERT_EQ(getInstructionPointer(context.get()), 0);
    
    // Act: Step forward one instruction
    bool success = stepInstruction(context.get());
    
    // Assert: Post conditions
    ASSERT_TRUE(success) << "Instruction should execute successfully";
    ASSERT_EQ(getStackDepth(context.get()), 1) << "Stack depth should increase by 1";
    ASSERT_EQ(getStackTop(context.get()), literals[0]) << "Stack top should equal literal[0]";
    ASSERT_EQ(getInstructionPointer(context.get()), 5) << "Instruction pointer should advance by 5 bytes";
}

TEST(BytecodeInstructions, PushLiteral_Index1) {
    // Arrange: PUSH_LITERAL with index 1
    std::vector<uint8_t> bytecode = encodeInstruction(0, {1}); // PUSH_LITERAL index 1
    
    std::vector<TaggedValue> literals = {
        makeSmallInteger(10),
        makeSmallInteger(20)
    };
    
    auto method = createCompiledMethod(bytecode, literals);
    auto context = createContext(method.get(), makeSmallInteger(0));
    
    // Act
    bool success = stepInstruction(context.get());
    
    // Assert
    ASSERT_TRUE(success);
    ASSERT_EQ(getStackDepth(context.get()), 1);
    ASSERT_EQ(getStackTop(context.get()), literals[1]) << "Should push literal[1]";
    ASSERT_EQ(getInstructionPointer(context.get()), 5);
}

// ============================================================================
// Test Runner Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
