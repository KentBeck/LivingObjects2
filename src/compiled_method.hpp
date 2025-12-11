#pragma once

#include "tagged_value.hpp"
#include <cstdint>
#include <cstddef>

/**
 * CompiledMethod - C++ representation of Smalltalk CompiledMethod object
 * 
 * Structure matches Smalltalk CompiledMethod:
 * - bytes: ByteArray (TaggedValue pointing to ByteArray object)
 * - literals: Array (TaggedValue pointing to Array object)
 * - numArgs: SmallInteger (number of arguments)
 * - numTemps: SmallInteger (number of temporary variables)
 * - primitiveNumber: SmallInteger (primitive method number, 0 if none)
 */
class CompiledMethod {
public:
    // Constructor
    CompiledMethod(TaggedValue bytes, TaggedValue literals,
                   uint32_t numArgs, uint32_t numTemps, uint32_t primitiveNumber);
    
    // Accessors for Smalltalk object fields
    TaggedValue getBytes() const { return bytes_; }
    TaggedValue getLiterals() const { return literals_; }
    uint32_t getNumArgs() const { return numArgs_; }
    uint32_t getNumTemps() const { return numTemps_; }
    uint32_t getPrimitiveNumber() const { return primitiveNumber_; }
    
    // Convenience methods for accessing bytecode (will work once ByteArray is implemented)
    // For now, these provide a way to access the underlying data
    // Note: These assume ByteArray and Array are implemented and accessible
    
private:
    TaggedValue bytes_;           // Points to ByteArray containing bytecode
    TaggedValue literals_;        // Points to Array containing literals
    uint32_t numArgs_;            // Number of arguments
    uint32_t numTemps_;           // Number of temporary variables
    uint32_t primitiveNumber_;    // Primitive method number (0 = no primitive)
};

