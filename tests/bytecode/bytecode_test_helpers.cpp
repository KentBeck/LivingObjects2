#include "bytecode_test_helpers.hpp"
#include "test_stubs.hpp"
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <memory>

// TODO: Include actual headers when they're implemented
// #include "src/tagged_value.hpp"
// #include "src/method.hpp"
// #include "src/context.hpp"
// #include "src/interpreter.hpp"

namespace test_helpers {

void encodeUint32LE(std::vector<uint8_t>& bytes, uint32_t value) {
    bytes.push_back(static_cast<uint8_t>(value & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

std::vector<uint8_t> encodeInstruction(uint8_t opcode, const std::vector<uint32_t>& operands) {
    std::vector<uint8_t> bytes;
    bytes.push_back(opcode);
    
    for (uint32_t operand : operands) {
        encodeUint32LE(bytes, operand);
    }
    
    return bytes;
}

// Placeholder implementations - these will be replaced with actual implementations
// once the VM classes are created

std::unique_ptr<CompiledMethod> createCompiledMethod(
    const std::vector<uint8_t>& bytecode,
    const std::vector<TaggedValue>& literals,
    uint32_t numArgs,
    uint32_t numTemps,
    uint32_t primitiveNumber
) {
    // Stub implementation - returns a CompiledMethod with the provided data
    return std::make_unique<CompiledMethod>(bytecode, literals, numArgs, numTemps, primitiveNumber);
}

std::unique_ptr<Context> createContext(
    CompiledMethod* method,
    TaggedValue receiver,
    const std::vector<TaggedValue>& /* args */
) {
    // Stub implementation - creates a Context with the method and receiver
    return std::make_unique<Context>(method, receiver);
}

bool stepInstruction(Context* context) {
    // TODO: Implement when Interpreter class exists
    // For now, just throw to indicate not implemented
    throw std::runtime_error("stepInstruction not yet implemented - need Interpreter");
}

uint32_t getInstructionPointer(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    return context->instructionPointer;
}

std::vector<TaggedValue> getStack(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    return context->stack;
}

TaggedValue getStackTop(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    if (context->stack.empty()) throw std::runtime_error("Stack is empty");
    return context->stack.back();
}

size_t getStackDepth(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    return context->stack.size();
}

TaggedValue makeSmallInteger(int32_t value) {
    // Stub: encode as (value << 2) | 0x03
    uintptr_t encoded = (static_cast<uintptr_t>(value) << 2) | 0x03;
    return TaggedValue(encoded);
}

TaggedValue makeNil() {
    return TaggedValue(0x00000001);
}

TaggedValue makeTrue() {
    return TaggedValue(0x00000005);
}

TaggedValue makeFalse() {
    return TaggedValue(0x00000009);
}

} // namespace test_helpers

