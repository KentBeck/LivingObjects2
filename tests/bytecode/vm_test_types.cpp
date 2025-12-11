#include "vm_test_types.hpp"

CompiledMethod* Context::getMethod() const {
    if (method.isNil() || !method.isPointer()) {
        return nullptr;
    }
    return reinterpret_cast<CompiledMethod*>(method.toPointer());
}

Array* Context::getStackArray() const {
    if (stack.isNil() || !stack.isPointer()) {
        return nullptr;
    }
    return Array::fromTaggedValue(stack);
}

uint32_t Context::getInstructionPointerValue() const {
    if (!instructionPointer.isSmallInteger()) {
        return 0;
    }
    int64_t value = instructionPointer.toSmallInteger();
    if (value < 0 || value > UINT32_MAX) {
        return 0;
    }
    return static_cast<uint32_t>(value);
}

void Context::setInstructionPointerValue(uint32_t ip) {
    instructionPointer = TaggedValue::fromSmallInteger(static_cast<int64_t>(ip));
}

