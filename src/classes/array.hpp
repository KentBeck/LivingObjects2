#pragma once

#include "../tagged_value.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>

/**
 * Array - C++ representation of Smalltalk Array object
 * 
 * Array stores a sequence of TaggedValue objects.
 * In Smalltalk, Array is used for:
 * - CompiledMethod literals storage
 * - General-purpose collections
 * - IdentityDictionary storage
 * 
 * Structure:
 * - Heap-allocated object (accessed via TaggedValue pointer)
 * - Contains a vector of TaggedValue elements
 */
class Array {
public:
    // Constructor from vector of TaggedValues
    explicit Array(const std::vector<TaggedValue>& elements);
    
    // Constructor with size (initialized to nil)
    explicit Array(size_t size);
    
    // Default constructor (empty Array)
    Array();
    
    // Accessors
    size_t size() const { return elements_.size(); }
    bool empty() const { return elements_.empty(); }
    
    // Get element at index
    TaggedValue get(size_t index) const;
    
    // Set element at index
    void set(size_t index, TaggedValue value);
    
    // Get all elements as vector (for convenience)
    const std::vector<TaggedValue>& getElements() const { return elements_; }
    
    // Convert to/from TaggedValue (as pointer)
    static TaggedValue toTaggedValue(Array* array);
    static Array* fromTaggedValue(TaggedValue value);
    
private:
    std::vector<TaggedValue> elements_;
};

