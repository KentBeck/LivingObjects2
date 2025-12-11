#pragma once

#include "tagged_value.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>

/**
 * ByteArray - C++ representation of Smalltalk ByteArray object
 * 
 * ByteArray stores a sequence of bytes (uint8_t values).
 * In Smalltalk, ByteArray is used for:
 * - CompiledMethod bytecode storage
 * - Binary data storage
 * 
 * Structure:
 * - Heap-allocated object (accessed via TaggedValue pointer)
 * - Contains a vector of bytes
 */
class ByteArray {
public:
    // Constructor from vector of bytes
    explicit ByteArray(const std::vector<uint8_t>& bytes);
    
    // Constructor from raw pointer and size
    ByteArray(const uint8_t* data, size_t size);
    
    // Default constructor (empty ByteArray)
    ByteArray();
    
    // Accessors
    size_t size() const { return bytes_.size(); }
    bool empty() const { return bytes_.empty(); }
    
    // Get byte at index
    uint8_t get(size_t index) const;
    
    // Set byte at index
    void set(size_t index, uint8_t value);
    
    // Get all bytes as vector (for convenience)
    const std::vector<uint8_t>& getBytes() const { return bytes_; }
    
    // Get raw pointer to data (for low-level access)
    const uint8_t* data() const { return bytes_.data(); }
    uint8_t* data() { return bytes_.data(); }
    
    // Convert to/from TaggedValue (as pointer)
    static TaggedValue toTaggedValue(ByteArray* byteArray);
    static ByteArray* fromTaggedValue(TaggedValue value);
    
private:
    std::vector<uint8_t> bytes_;
};

