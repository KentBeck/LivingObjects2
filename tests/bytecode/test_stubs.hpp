#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

// Minimal stub types for testing
// These will be replaced with real implementations when VM classes are created

struct TaggedValue {
    uintptr_t value;
    TaggedValue() : value(0) {}
    TaggedValue(uintptr_t v) : value(v) {}
    bool operator==(const TaggedValue& other) const { return value == other.value; }
    bool operator!=(const TaggedValue& other) const { return value != other.value; }
};

// Minimal stub CompiledMethod
class CompiledMethod {
public:
    std::vector<uint8_t> bytecode;
    std::vector<TaggedValue> literals;
    uint32_t numArgs;
    uint32_t numTemps;
    uint32_t primitiveNumber;
    
    CompiledMethod(const std::vector<uint8_t>& bc, const std::vector<TaggedValue>& lit,
                   uint32_t na, uint32_t nt, uint32_t pn)
        : bytecode(bc), literals(lit), numArgs(na), numTemps(nt), primitiveNumber(pn) {}
};

// Minimal stub Context
class Context {
public:
    CompiledMethod* method;
    TaggedValue receiver;
    std::vector<TaggedValue> stack;
    uint32_t instructionPointer;
    
    Context(CompiledMethod* m, TaggedValue r) 
        : method(m), receiver(r), instructionPointer(0) {}
};

