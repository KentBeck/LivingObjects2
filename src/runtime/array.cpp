#include "array.hpp"
#include <stdexcept>

namespace runtime {

Array::Array(const std::vector<TaggedValue>& elements)
    : elements_(elements) {
}

Array::Array(size_t size)
    : elements_(size, TaggedValue::nil()) {
}

Array::Array()
    : elements_() {
}

TaggedValue Array::get(size_t index) const {
    if (index >= elements_.size()) {
        throw std::out_of_range("Array index out of range");
    }
    return elements_[index];
}

void Array::set(size_t index, TaggedValue value) {
    if (index >= elements_.size()) {
        throw std::out_of_range("Array index out of range");
    }
    elements_[index] = value;
}

TaggedValue Array::pop() {
    if (elements_.empty()) {
        throw std::runtime_error("Array pop on empty");
    }
    TaggedValue v = elements_.back();
    elements_.pop_back();
    return v;
}

TaggedValue Array::back() const {
    if (elements_.empty()) {
        throw std::runtime_error("Array back on empty");
    }
    return elements_.back();
}

TaggedValue Array::toTaggedValue(Array* array) {
    if (array == nullptr) {
        return TaggedValue::nil();
    }
    return TaggedValue::fromPointer(array);
}

Array* Array::fromTaggedValue(TaggedValue value) {
    if (value.isNil() || !value.isPointer()) {
        return nullptr;
    }
    return reinterpret_cast<Array*>(value.toPointer());
}

} // namespace runtime

