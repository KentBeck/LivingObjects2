#pragma once

#include "../tagged_value.hpp"
#include "byte_array.hpp"
#include "array.hpp"
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
                   TaggedValue numArgs, TaggedValue numTemps, TaggedValue primitiveNumber);
    
    // Accessors for Smalltalk object fields
    TaggedValue getBytes() const { return bytes_; }
    TaggedValue getLiterals() const { return literals_; }
    TaggedValue getNumArgs() const { return numArgs_; }
    TaggedValue getNumTemps() const { return numTemps_; }
    TaggedValue getPrimitiveNumber() const { return primitiveNumber_; }
    
    // Convenience methods for accessing ByteArray
    ByteArray* getByteArray() const;
    size_t getBytecodeSize() const;
    uint8_t getBytecodeAt(size_t index) const;
    
    // Convenience methods for accessing Array
    Array* getArray() const;
    size_t getLiteralsSize() const;
    TaggedValue getLiteralAt(size_t index) const;
    
private:
    TaggedValue bytes_;           // Points to ByteArray containing bytecode
    TaggedValue literals_;        // Points to Array containing literals
    TaggedValue numArgs_;        // Number of arguments (SmallInteger)
    TaggedValue numTemps_;        // Number of temporary variables (SmallInteger)
    TaggedValue primitiveNumber_; // Primitive method number (SmallInteger, 0 = no primitive)
};

