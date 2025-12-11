#pragma once

#include "../src/tagged_value.hpp"  // Real TaggedValue implementation
#include <vector>
#include <cstdint>
#include <cstddef>

// Temporary stub types for testing
// TaggedValue: REAL implementation in src/tagged_value.hpp ✓
// CompiledMethod: STUB - will be replaced in Phase 2.5
// Context: STUB - will be replaced in Phase 3.1

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

