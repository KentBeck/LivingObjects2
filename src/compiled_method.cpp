#include "compiled_method.hpp"

CompiledMethod::CompiledMethod(TaggedValue bytes, TaggedValue literals,
                               uint32_t numArgs, uint32_t numTemps, uint32_t primitiveNumber)
    : bytes_(bytes), literals_(literals), numArgs_(numArgs), 
      numTemps_(numTemps), primitiveNumber_(primitiveNumber) {
}

