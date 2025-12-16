#pragma once

#include "../tagged_value.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>

namespace runtime {

/**
 * ByteArray - TEMPORARY runtime/test backing store for indexed bytes.
 * 
 * This is NOT a Smalltalk-mirror layout type. Do not put this in src/classes/.
 * Mirror types in src/classes/ store only TaggedValue slots; indexed storage will
 * be handled by object memory later. For now, tests need a simple byte container.
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

} // namespace runtime

