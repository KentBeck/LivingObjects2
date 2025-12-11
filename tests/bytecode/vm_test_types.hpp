#pragma once

#include "../src/tagged_value.hpp"  // Real TaggedValue implementation
#include "../src/compiled_method.hpp"  // Real CompiledMethod implementation
#include <vector>
#include <cstdint>
#include <cstddef>

// Real implementations:
// TaggedValue: REAL implementation in src/tagged_value.hpp ✓
// CompiledMethod: REAL implementation in src/compiled_method.hpp ✓
// Context: STUB - will be replaced in Phase 3.1

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

