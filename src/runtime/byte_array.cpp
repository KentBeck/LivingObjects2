#include "byte_array.hpp"
#include <stdexcept>
#include <cstring>

namespace runtime {

ByteArray::ByteArray(const std::vector<uint8_t>& bytes)
    : bytes_(bytes) {
}

ByteArray::ByteArray(const uint8_t* data, size_t size)
    : bytes_(data, data + size) {
}

ByteArray::ByteArray()
    : bytes_() {
}

uint8_t ByteArray::get(size_t index) const {
    if (index >= bytes_.size()) {
        throw std::out_of_range("ByteArray index out of range");
    }
    return bytes_[index];
}

void ByteArray::set(size_t index, uint8_t value) {
    if (index >= bytes_.size()) {
        throw std::out_of_range("ByteArray index out of range");
    }
    bytes_[index] = value;
}

TaggedValue ByteArray::toTaggedValue(ByteArray* byteArray) {
    if (byteArray == nullptr) {
        return TaggedValue::nil();
    }
    return TaggedValue::fromPointer(byteArray);
}

ByteArray* ByteArray::fromTaggedValue(TaggedValue value) {
    if (value.isNil() || !value.isPointer()) {
        return nullptr;
    }
    return reinterpret_cast<ByteArray*>(value.toPointer());
}

} // namespace runtime

