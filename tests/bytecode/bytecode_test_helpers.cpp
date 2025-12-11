#include "bytecode_test_helpers.hpp"
#include "../src/classes/context.hpp"
#include "../src/tagged_value.hpp"
#include "../src/classes/compiled_method.hpp"
#include "../src/classes/byte_array.hpp"
#include "../src/classes/array.hpp"
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace test_helpers {

// Storage for Array objects used in Context stacks
// This is a temporary solution until we have proper memory management
static std::unordered_map<const Context*, std::unique_ptr<Array>> stack_storage;

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
    
    // Create Array from literals vector
    auto array = std::make_unique<Array>(literals);
    TaggedValue literalsTagged = Array::toTaggedValue(array.get());
    
    // Create CompiledMethod with ByteArray for bytes and Array for literals
    // Convert uint32_t parameters to TaggedValue SmallIntegers
    // Store byteArray and array in a way that they persist (we'll manage them with the CompiledMethod)
    // For now, we'll keep them alive by storing them alongside the method
    // TODO: Once we have proper memory management, ByteArray and Array will be heap-allocated
    auto method = std::make_unique<CompiledMethod>(
        bytesTagged,    // bytes (ByteArray)
        literalsTagged, // literals (Array)
        TaggedValue::fromSmallInteger(static_cast<int64_t>(numArgs)),        // numArgs (SmallInteger)
        TaggedValue::fromSmallInteger(static_cast<int64_t>(numTemps)),      // numTemps (SmallInteger)
        TaggedValue::fromSmallInteger(static_cast<int64_t>(primitiveNumber)) // primitiveNumber (SmallInteger)
    );
    
    // Keep byteArray and array alive by storing them with the method
    // This is a temporary solution until we have proper memory management
    // In a real VM, ByteArray and Array would be heap-allocated and managed by the GC
    static std::unordered_map<const CompiledMethod*, std::unique_ptr<ByteArray>> byteArray_storage;
    static std::unordered_map<const CompiledMethod*, std::unique_ptr<Array>> array_storage;
    byteArray_storage[method.get()] = std::move(byteArray);
    array_storage[method.get()] = std::move(array);
    
    return method;
}

std::unique_ptr<Context> createContext(
    CompiledMethod* method,
    TaggedValue receiver,
    const std::vector<TaggedValue>& /* args */
) {
    // Convert CompiledMethod* to TaggedValue
    TaggedValue methodTagged = TaggedValue::fromPointer(method);
    
    // Create empty Array for stack
    auto stackArray = std::make_unique<Array>();
    TaggedValue stackTagged = Array::toTaggedValue(stackArray.get());
    
    // Create Context with Smalltalk objects
    auto context = std::make_unique<Context>(methodTagged, receiver);
    context->stack = stackTagged;
    
    // Keep stackArray alive by storing it with the context
    // This is a temporary solution until we have proper memory management
    stack_storage[context.get()] = std::move(stackArray);
    
    return context;
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
    if (!context) {
        return false;
    }
    
    // Get CompiledMethod from TaggedValue
    CompiledMethod* method = context->getMethod();
    if (method == nullptr) {
        return false;
    }
    
    // Get ByteArray from CompiledMethod
    ByteArray* byteArray = method->getByteArray();
    if (byteArray == nullptr) {
        return false; // No bytecode
    }
    
    uint32_t ip = context->getInstructionPointerValue();
    
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
        
        // Get Array from CompiledMethod
        Array* array = method->getArray();
        if (array == nullptr) {
            return false; // No literals
        }
        
        // Check index bounds
        if (index >= array->size()) {
            return false; // Index out of bounds
        }
        
        // Get stack Array
        Array* stackArray = context->getStackArray();
        if (stackArray == nullptr) {
            return false; // No stack
        }
        
        // Push literal onto stack
        TaggedValue literal = array->get(index);
        // Get current stack size and add new element
        size_t stackSize = stackArray->size();
        // Resize stack if needed (Array needs to be resizable)
        // For now, we'll create a new array with the new element
        std::vector<TaggedValue> stackElements;
        for (size_t i = 0; i < stackSize; i++) {
            stackElements.push_back(stackArray->get(i));
        }
        stackElements.push_back(literal);
        
        // Update stack Array
        // We need to replace the stack Array with a new one containing the new element
        // This is a temporary solution - in a real VM, Array would support dynamic resizing
        auto newStackArray = std::make_unique<Array>(stackElements);
        context->stack = Array::toTaggedValue(newStackArray.get());
        
        // Keep new stack array alive (replaces old one)
        stack_storage[context] = std::move(newStackArray);
        
        // Advance instruction pointer by 5 bytes (1 opcode + 4 index)
        context->setInstructionPointerValue(ip + 5);
        
        return true;
    }
    
    // Unknown opcode
    return false;
}

uint32_t getInstructionPointer(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    return context->getInstructionPointerValue();
}

std::vector<TaggedValue> getStack(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    Array* stackArray = context->getStackArray();
    if (stackArray == nullptr) {
        return std::vector<TaggedValue>();
    }
    std::vector<TaggedValue> result;
    for (size_t i = 0; i < stackArray->size(); i++) {
        result.push_back(stackArray->get(i));
    }
    return result;
}

TaggedValue getStackTop(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    Array* stackArray = context->getStackArray();
    if (stackArray == nullptr || stackArray->empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stackArray->get(stackArray->size() - 1);
}

size_t getStackDepth(Context* context) {
    if (!context) throw std::runtime_error("Context is null");
    Array* stackArray = context->getStackArray();
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
