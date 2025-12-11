#include "bytecode_test_helpers.hpp"
#include "vm_test_types.hpp"
#include "../src/tagged_value.hpp"
#include "../src/compiled_method.hpp"
#include "../src/byte_array.hpp"
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace test_helpers {

// Temporary storage for literals until Array is implemented
// ByteArray is now implemented, so bytecode is accessed through CompiledMethod's ByteArray
static std::unordered_map<const CompiledMethod*, std::vector<TaggedValue>> literals_storage;

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
    // Create ByteArray from bytecode vector
    auto byteArray = std::make_unique<ByteArray>(bytecode);
    TaggedValue bytesTagged = ByteArray::toTaggedValue(byteArray.get());
    
    // Create CompiledMethod with ByteArray for bytes
    // Store byteArray in a way that it persists (we'll manage it with the CompiledMethod)
    // For now, we'll keep the byteArray alive by storing it alongside the method
    // TODO: Once we have proper memory management, ByteArray will be heap-allocated
    auto method = std::make_unique<CompiledMethod>(
        bytesTagged,  // bytes (ByteArray)
        TaggedValue::nil(),  // literals (will be Array once implemented)
        numArgs,
        numTemps,
        primitiveNumber
    );
    
    // Store literals in temporary map (until Array is implemented)
    literals_storage[method.get()] = literals;
    
    // Keep byteArray alive by storing it with the method
    // This is a temporary solution until we have proper memory management
    // In a real VM, ByteArray would be heap-allocated and managed by the GC
    static std::unordered_map<const CompiledMethod*, std::unique_ptr<ByteArray>> byteArray_storage;
    byteArray_storage[method.get()] = std::move(byteArray);
    
    return method;
}

std::unique_ptr<Context> createContext(
    CompiledMethod* method,
    TaggedValue receiver,
    const std::vector<TaggedValue>& /* args */
) {
    return std::make_unique<Context>(method, receiver);
}

// Read a 32-bit little-endian value from ByteArray
static uint32_t readUint32LEFromByteArray(ByteArray* byteArray, uint32_t offset) {
    if (offset + 4 > byteArray->size()) {
        throw std::runtime_error("Bytecode read out of bounds");
    }
    return static_cast<uint32_t>(byteArray->get(offset)) |
           (static_cast<uint32_t>(byteArray->get(offset + 1)) << 8) |
           (static_cast<uint32_t>(byteArray->get(offset + 2)) << 16) |
           (static_cast<uint32_t>(byteArray->get(offset + 3)) << 24);
}

bool stepInstruction(Context* context) {
    if (!context || !context->method) {
        return false;
    }
    
    // Get ByteArray from CompiledMethod
    ByteArray* byteArray = context->method->getByteArray();
    if (byteArray == nullptr) {
        return false; // No bytecode
    }
    
    uint32_t ip = context->instructionPointer;
    
    // Check bounds
    if (ip >= byteArray->size()) {
        return false;
    }
    
    uint8_t opcode = byteArray->get(ip);
    
    // PUSH_LITERAL (opcode 0)
    // Format: 00 [index:uint32_le]
    // Stack: ... → ..., literal
    if (opcode == 0) {
        // Read 4-byte index
        if (ip + 5 > byteArray->size()) {
            return false; // Not enough bytes
        }
        
        uint32_t index = readUint32LEFromByteArray(byteArray, ip + 1);
        
        // Get literals from temporary storage (until Array is implemented)
        auto literals_it = literals_storage.find(context->method);
        if (literals_it == literals_storage.end()) {
            return false; // No literals stored
        }
        const auto& literals = literals_it->second;
        
        // Check index bounds
        if (index >= literals.size()) {
            return false; // Index out of bounds
        }
        
        // Push literal onto stack
        TaggedValue literal = literals[index];
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
