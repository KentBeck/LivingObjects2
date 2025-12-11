#include "bytecode_test_helpers.hpp"
#include "vm_test_types.hpp"
#include "../src/tagged_value.hpp"
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <memory>

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

std::unique_ptr<CompiledMethod> createCompiledMethod(
    const std::vector<uint8_t>& bytecode,
    const std::vector<TaggedValue>& literals,
    uint32_t numArgs,
    uint32_t numTemps,
    uint32_t primitiveNumber
) {
    return std::make_unique<CompiledMethod>(bytecode, literals, numArgs, numTemps, primitiveNumber);
}

std::unique_ptr<Context> createContext(
    CompiledMethod* method,
    TaggedValue receiver,
    const std::vector<TaggedValue>& /* args */
) {
    return std::make_unique<Context>(method, receiver);
}

// Read a 32-bit little-endian value from bytecode
static uint32_t readUint32LE(const std::vector<uint8_t>& bytecode, uint32_t offset) {
    if (offset + 4 > bytecode.size()) {
        throw std::runtime_error("Bytecode read out of bounds");
    }
    return static_cast<uint32_t>(bytecode[offset]) |
           (static_cast<uint32_t>(bytecode[offset + 1]) << 8) |
           (static_cast<uint32_t>(bytecode[offset + 2]) << 16) |
           (static_cast<uint32_t>(bytecode[offset + 3]) << 24);
}

bool stepInstruction(Context* context) {
    if (!context || !context->method) {
        return false;
    }
    
    const auto& bytecode = context->method->bytecode;
    uint32_t ip = context->instructionPointer;
    
    // Check bounds
    if (ip >= bytecode.size()) {
        return false;
    }
    
    uint8_t opcode = bytecode[ip];
    
    // PUSH_LITERAL (opcode 0)
    // Format: 00 [index:uint32_le]
    // Stack: ... → ..., literal
    if (opcode == 0) {
        // Read 4-byte index
        if (ip + 5 > bytecode.size()) {
            return false; // Not enough bytes
        }
        
        uint32_t index = readUint32LE(bytecode, ip + 1);
        
        // Check index bounds
        if (index >= context->method->literals.size()) {
            return false; // Index out of bounds
        }
        
        // Push literal onto stack
        TaggedValue literal = context->method->literals[index];
        context->stack.push_back(literal);
        
        // Advance instruction pointer by 5 bytes (1 opcode + 4 index)
        context->instructionPointer = ip + 5;
        
        return true;
    }
    
    // Unknown opcode
    return false;
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

TaggedValue makeSmallInteger(int64_t value) {
    return TaggedValue::fromSmallInteger(value);
}

TaggedValue makeNil() {
    return TaggedValue::nil();
}

TaggedValue makeTrue() {
    return TaggedValue::trueValue();
}

TaggedValue makeFalse() {
    return TaggedValue::falseValue();
}

} // namespace test_helpers
