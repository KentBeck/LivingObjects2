#pragma once

#include "mirror.hpp"

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
namespace st {

class CompiledMethod {
public:
    // Constructor
    CompiledMethod(TaggedValue bytes, TaggedValue literals, TaggedValue numArgs, TaggedValue numTemps, TaggedValue primitiveNumber);
    
    // Accessors for Smalltalk object fields
    TaggedValue getBytes() const { return bytes_; }
    TaggedValue getLiterals() const { return literals_; }
    TaggedValue getNumArgs() const { return numArgs_; }
    TaggedValue getNumTemps() const { return numTemps_; }
    TaggedValue getPrimitiveNumber() const { return primitiveNumber_; }
    
private:
    ST_SLOT(bytes_);            // ByteArray (object pointer)
    ST_SLOT(literals_);         // Array (object pointer)
    ST_SLOT(numArgs_);          // SmallInteger
    ST_SLOT(numTemps_);         // SmallInteger
    ST_SLOT(primitiveNumber_);  // SmallInteger
};

} // namespace st

