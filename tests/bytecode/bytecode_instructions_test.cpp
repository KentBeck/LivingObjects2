#include "bytecode_test_helpers.hpp"
#include "test_stubs.hpp"
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

TEST(BytecodeInstructions, PushLiteral_NonEmptyStack) {
    // Arrange: Start with items on stack
    std::vector<uint8_t> bytecode = encodeInstruction(0, {0});
    std::vector<TaggedValue> literals = {makeSmallInteger(99)};
    
    auto method = createCompiledMethod(bytecode, literals);
    auto context = createContext(method.get(), makeSmallInteger(0));
    
    // Pre-populate stack (this would be done via helper or previous instructions)
    // For now, we'll test that the stack grows correctly
    
    // Act
    bool success = stepInstruction(context.get());
    
    // Assert
    ASSERT_TRUE(success);
    auto stack = getStack(context.get());
    ASSERT_EQ(stack.size(), 1);
    ASSERT_EQ(stack[0], literals[0]);
}

TEST(BytecodeInstructions, PushLiteral_IndexOutOfBounds) {
    // Arrange: Try to access literal index that doesn't exist
    std::vector<uint8_t> bytecode = encodeInstruction(0, {5}); // Index 5
    std::vector<TaggedValue> literals = {makeSmallInteger(42)}; // Only 1 literal
    
    auto method = createCompiledMethod(bytecode, literals);
    auto context = createContext(method.get(), makeSmallInteger(0));
    
    // Act
    bool success = stepInstruction(context.get());
    
    // Assert: Should fail with IndexError
    ASSERT_FALSE(success) << "Should fail when literal index is out of bounds";
    // TODO: Check for specific error type (IndexError)
}

// ============================================================================
// PUSH_SELF (opcode 3)
// ============================================================================

/**
 * Test PUSH_SELF instruction
 * 
 * Format: 03
 * Stack: ... → ..., self
 * 
 * Post conditions:
 * - Stack depth increased by 1
 * - Top of stack equals context's self
 * - Instruction pointer advanced by 1
 */
TEST(BytecodeInstructions, PushSelf_Basic) {
    // Arrange
    std::vector<uint8_t> bytecode = {3}; // PUSH_SELF (no operands)
    std::vector<TaggedValue> literals = {};
    
    auto method = createCompiledMethod(bytecode, literals);
    TaggedValue receiver = makeSmallInteger(42);
    auto context = createContext(method.get(), receiver);
    
    // Pre-conditions
    ASSERT_EQ(getStackDepth(context.get()), 0);
    
    // Act
    bool success = stepInstruction(context.get());
    
    // Assert
    ASSERT_TRUE(success);
    ASSERT_EQ(getStackDepth(context.get()), 1);
    ASSERT_EQ(getStackTop(context.get()), receiver) << "Stack top should equal self (receiver)";
    ASSERT_EQ(getInstructionPointer(context.get()), 1) << "Instruction pointer should advance by 1 byte";
}

// ============================================================================
// POP (opcode 11)
// ============================================================================

/**
 * Test POP instruction
 * 
 * Format: 0B
 * Stack: ..., value → ...
 * 
 * Post conditions:
 * - Stack depth decreased by 1
 * - Value removed from stack
 * - Instruction pointer advanced by 1
 */
TEST(BytecodeInstructions, Pop_Basic) {
    // Arrange: Create bytecode that pushes a value then pops it
    std::vector<uint8_t> bytecode;
    auto pushLiteral = encodeInstruction(0, {0}); // PUSH_LITERAL 0
    bytecode.insert(bytecode.end(), pushLiteral.begin(), pushLiteral.end());
    bytecode.push_back(11); // POP
    
    std::vector<TaggedValue> literals = {makeSmallInteger(42)};
    
    auto method = createCompiledMethod(bytecode, literals);
    auto context = createContext(method.get(), makeSmallInteger(0));
    
    // Execute PUSH_LITERAL first
    stepInstruction(context.get());
    ASSERT_EQ(getStackDepth(context.get()), 1);
    
    // Act: Execute POP
    bool success = stepInstruction(context.get());
    
    // Assert
    ASSERT_TRUE(success);
    ASSERT_EQ(getStackDepth(context.get()), 0) << "Stack should be empty after POP";
    ASSERT_EQ(getInstructionPointer(context.get()), 6) << "IP should be at byte 6 (5 for PUSH_LITERAL + 1 for POP)";
}

TEST(BytecodeInstructions, Pop_StackUnderflow) {
    // Arrange: Try to POP from empty stack
    std::vector<uint8_t> bytecode = {11}; // POP
    std::vector<TaggedValue> literals = {};
    
    auto method = createCompiledMethod(bytecode, literals);
    auto context = createContext(method.get(), makeSmallInteger(0));
    
    // Pre-condition: Stack is empty
    ASSERT_EQ(getStackDepth(context.get()), 0);
    
    // Act
    bool success = stepInstruction(context.get());
    
    // Assert: Should fail with StackUnderflowError
    ASSERT_FALSE(success) << "Should fail when popping from empty stack";
}

// ============================================================================
// Test Runner Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

