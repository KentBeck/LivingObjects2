#pragma once

#include "../src/tagged_value.hpp"  // Real TaggedValue implementation
#include "../src/compiled_method.hpp"  // Real CompiledMethod implementation
#include "../src/array.hpp"  // Real Array implementation
#include <vector>
#include <cstdint>
#include <cstddef>

// Real implementations:
// TaggedValue: REAL implementation in src/tagged_value.hpp ✓
// CompiledMethod: REAL implementation in src/compiled_method.hpp ✓
// Array: REAL implementation in src/array.hpp ✓
// Context: STUB - will be replaced in Phase 3.1

// Minimal stub Context (using Smalltalk objects)
class Context {
public:
    TaggedValue method;        // Points to CompiledMethod
    TaggedValue receiver;      // Receiver object
    TaggedValue stack;         // Points to Array containing stack
    TaggedValue instructionPointer; // Instruction pointer (SmallInteger)
    
    Context(TaggedValue m, TaggedValue r) 
        : method(m), receiver(r), 
          stack(TaggedValue::nil()),  // Stack will be initialized as empty Array
          instructionPointer(TaggedValue::fromSmallInteger(0)) {}
    
    // Convenience methods to access objects
    CompiledMethod* getMethod() const;
    Array* getStackArray() const;
    uint32_t getInstructionPointerValue() const;
    void setInstructionPointerValue(uint32_t ip);
};

