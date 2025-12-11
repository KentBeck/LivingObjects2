#include "array.hpp"
#include <stdexcept>

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

