#include "compiled_method.hpp"

CompiledMethod::CompiledMethod(TaggedValue bytes, TaggedValue literals,
                               uint32_t numArgs, uint32_t numTemps, uint32_t primitiveNumber)
    : bytes_(bytes), literals_(literals), numArgs_(numArgs), 
      numTemps_(numTemps), primitiveNumber_(primitiveNumber) {
}

ByteArray* CompiledMethod::getByteArray() const {
    return ByteArray::fromTaggedValue(bytes_);
}

size_t CompiledMethod::getBytecodeSize() const {
    ByteArray* byteArray = getByteArray();
    if (byteArray == nullptr) {
        return 0;
    }
    return byteArray->size();
}

uint8_t CompiledMethod::getBytecodeAt(size_t index) const {
    ByteArray* byteArray = getByteArray();
    if (byteArray == nullptr) {
        return 0;
    }
    return byteArray->get(index);
}

