#pragma once

#include "../tagged_value.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>

namespace runtime {

/**
 * Array - TEMPORARY runtime/test backing store for indexed objects.
 * 
 * This is NOT a Smalltalk-mirror layout type. Do not put this in src/classes/.
 * Mirror types in src/classes/ store only TaggedValue slots; indexed storage will
 * be handled by object memory later. For now, tests need a simple container.
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

    // Push/pop for test stacks (dynamic growth)
    void push(TaggedValue value) { elements_.push_back(value); }
    TaggedValue pop();
    TaggedValue back() const;
    
    // Get all elements as vector (for convenience)
    const std::vector<TaggedValue>& getElements() const { return elements_; }
    
    // Convert to/from TaggedValue (as pointer)
    static TaggedValue toTaggedValue(Array* array);
    static Array* fromTaggedValue(TaggedValue value);
    
private:
    std::vector<TaggedValue> elements_;
};

} // namespace runtime

