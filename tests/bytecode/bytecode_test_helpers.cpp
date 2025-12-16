#include "bytecode_test_helpers.hpp"
#include "../src/classes/context.hpp"
#include "../src/tagged_value.hpp"
#include "../src/classes/compiled_method.hpp"
#include "../src/runtime/byte_array.hpp"
#include "../src/runtime/array.hpp"
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace test_helpers {

// Storage for runtime Arrays used in Context stacks
// This is a temporary solution until we have proper memory management
static std::unordered_map<const st::Context*, std::unique_ptr<runtime::Array>> stack_storage;

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

std::unique_ptr<st::CompiledMethod> createCompiledMethod(
    const std::vector<uint8_t>& bytecode,
    const std::vector<TaggedValue>& literals,
    uint32_t numArgs,
    uint32_t numTemps,
    uint32_t primitiveNumber
) {
    // Create runtime ByteArray from bytecode vector
    auto byteArray = std::make_unique<runtime::ByteArray>(bytecode);
    TaggedValue bytesTagged = runtime::ByteArray::toTaggedValue(byteArray.get());
    
    // Create runtime Array from literals vector
    auto array = std::make_unique<runtime::Array>(literals);
    TaggedValue literalsTagged = runtime::Array::toTaggedValue(array.get());
    
    // Create CompiledMethod with ByteArray for bytes and Array for literals
    // Convert uint32_t parameters to TaggedValue SmallIntegers
    // Store byteArray and array in a way that they persist (we'll manage them with the CompiledMethod)
    // For now, we'll keep them alive by storing them alongside the method
    // TODO: Once we have proper memory management, ByteArray and Array will be heap-allocated
    auto method = std::make_unique<st::CompiledMethod>(
        bytesTagged,    // bytes (ByteArray)
        literalsTagged, // literals (Array)
        TaggedValue::fromSmallInteger(static_cast<int64_t>(numArgs)),        // numArgs (SmallInteger)
        TaggedValue::fromSmallInteger(static_cast<int64_t>(numTemps)),      // numTemps (SmallInteger)
        TaggedValue::fromSmallInteger(static_cast<int64_t>(primitiveNumber)) // primitiveNumber (SmallInteger)
    );
    
    // Keep byteArray and array alive by storing them with the method
    // This is a temporary solution until we have proper memory management
    // In a real VM, ByteArray and Array would be heap-allocated and managed by the GC
    static std::unordered_map<const st::CompiledMethod*, std::unique_ptr<runtime::ByteArray>> byteArray_storage;
    static std::unordered_map<const st::CompiledMethod*, std::unique_ptr<runtime::Array>> array_storage;
    byteArray_storage[method.get()] = std::move(byteArray);
    array_storage[method.get()] = std::move(array);
    
    return method;
}

std::unique_ptr<st::Context> createContext(
    st::CompiledMethod* method,
    TaggedValue receiver,
    const std::vector<TaggedValue>& /* args */
) {
    // Convert CompiledMethod* to TaggedValue
    TaggedValue methodTagged = TaggedValue::fromPointer(method);
    
    // Create empty runtime Array for stack
    auto stackArray = std::make_unique<runtime::Array>();
    TaggedValue stackTagged = runtime::Array::toTaggedValue(stackArray.get());
    
    // Create Context with Smalltalk objects
    auto context = std::make_unique<st::Context>(
        methodTagged,
        receiver,
        stackTagged,
        TaggedValue::fromSmallInteger(0)
    );
    
    // Keep stackArray alive by storing it with the context
    // This is a temporary solution until we have proper memory management
    stack_storage[context.get()] = std::move(stackArray);
    
    return context;
}

// Read a 32-bit little-endian value from runtime ByteArray
static uint32_t readUint32LEFromByteArray(runtime::ByteArray* byteArray, uint32_t offset) {
    if (offset + 4 > byteArray->size()) {
        throw std::runtime_error("Bytecode read out of bounds");
    }
    return static_cast<uint32_t>(byteArray->get(offset)) |
           (static_cast<uint32_t>(byteArray->get(offset + 1)) << 8) |
           (static_cast<uint32_t>(byteArray->get(offset + 2)) << 16) |
           (static_cast<uint32_t>(byteArray->get(offset + 3)) << 24);
}

static st::CompiledMethod* methodFromTagged(TaggedValue v) {
    if (v.isNil() || !v.isPointer()) return nullptr;
    return reinterpret_cast<st::CompiledMethod*>(v.toPointer());
}

bool stepInstruction(st::Context* context) {
    if (!context) {
        return false;
    }
    
    // Get CompiledMethod from TaggedValue slot
    st::CompiledMethod* method = methodFromTagged(context->method());
    if (method == nullptr) {
        return false;
    }
    
    // Decode runtime ByteArray from bytes slot
    runtime::ByteArray* byteArray = runtime::ByteArray::fromTaggedValue(method->getBytes());
    if (byteArray == nullptr) {
        return false; // No bytecode
    }
    
    if (!context->instructionPointer().isSmallInteger()) return false;
    uint32_t ip = static_cast<uint32_t>(context->instructionPointer().toSmallInteger());
    
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
        
        // Decode runtime Array from literals slot
        runtime::Array* array = runtime::Array::fromTaggedValue(method->getLiterals());
        if (array == nullptr) {
            return false; // No literals
        }
        
        // Check index bounds
        if (index >= array->size()) {
            return false; // Index out of bounds
        }
        
        // Decode runtime Array from stack slot
        runtime::Array* stackArray = runtime::Array::fromTaggedValue(context->stack());
        if (stackArray == nullptr) {
            return false; // No stack
        }
        
        // Push literal onto stack
        TaggedValue literal = array->get(index);
        stackArray->push(literal);
        
        // Advance instruction pointer by 5 bytes (1 opcode + 4 index)
        context->setInstructionPointer(TaggedValue::fromSmallInteger(static_cast<int64_t>(ip + 5)));
        
        return true;
    }
    
    // Unknown opcode
    return false;
}

uint32_t getInstructionPointer(st::Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    if (!context->instructionPointer().isSmallInteger()) return 0;
    return static_cast<uint32_t>(context->instructionPointer().toSmallInteger());
}

std::vector<TaggedValue> getStack(st::Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    runtime::Array* stackArray = runtime::Array::fromTaggedValue(context->stack());
    if (stackArray == nullptr) {
        return std::vector<TaggedValue>();
    }
    std::vector<TaggedValue> result;
    for (size_t i = 0; i < stackArray->size(); i++) {
        result.push_back(stackArray->get(i));
    }
    return result;
}

TaggedValue getStackTop(st::Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    runtime::Array* stackArray = runtime::Array::fromTaggedValue(context->stack());
    if (stackArray == nullptr || stackArray->empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stackArray->back();
}

size_t getStackDepth(st::Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    runtime::Array* stackArray = runtime::Array::fromTaggedValue(context->stack());
    if (stackArray == nullptr) {
        return 0;
    }
    return stackArray->size();
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
