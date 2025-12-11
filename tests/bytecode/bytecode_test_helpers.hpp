#pragma once

#include "../src/tagged_value.hpp"
#include "../src/classes/context.hpp"  // For CompiledMethod and Context
#include <vector>
#include <cstdint>
#include <memory>

// Forward declarations
class VM;

namespace test_helpers {

/**
 * Create a CompiledMethod in memory with specified bytecode and literals
 */
std::unique_ptr<CompiledMethod> createCompiledMethod(
    const std::vector<uint8_t>& bytecode,
    const std::vector<TaggedValue>& literals = {},
    uint32_t numArgs = 0,
    uint32_t numTemps = 0,
    uint32_t primitiveNumber = 0
);

/**
 * Create a Context for executing a method
 */
std::unique_ptr<Context> createContext(
    CompiledMethod* method,
    TaggedValue receiver,
    const std::vector<TaggedValue>& args = {}
);

/**
 * Step forward one instruction in the context
 * Returns true if instruction executed successfully, false otherwise
 */
bool stepInstruction(Context* context);

/**
 * Get the current instruction pointer
 */
uint32_t getInstructionPointer(Context* context);

/**
 * Get the stack as a vector (for testing)
 */
std::vector<TaggedValue> getStack(Context* context);

/**
 * Get the top of the stack
 */
TaggedValue getStackTop(Context* context);

/**
 * Get the stack depth
 */
size_t getStackDepth(Context* context);

/**
 * Create a TaggedValue from a SmallInteger (64-bit)
 */
TaggedValue makeSmallInteger(int64_t value);

/**
 * Create a TaggedValue for nil
 */
TaggedValue makeNil();

/**
 * Create a TaggedValue for true
 */
TaggedValue makeTrue();

/**
 * Create a TaggedValue for false
 */
TaggedValue makeFalse();

/**
 * Encode a bytecode instruction with operands
 */
std::vector<uint8_t> encodeInstruction(uint8_t opcode, const std::vector<uint32_t>& operands = {});

/**
 * Encode a 32-bit little-endian value
 */
void encodeUint32LE(std::vector<uint8_t>& bytes, uint32_t value);

} // namespace test_helpers
