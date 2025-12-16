#include "compiled_method.hpp"

namespace st {

CompiledMethod::CompiledMethod(TaggedValue bytes, TaggedValue literals,
                               TaggedValue numArgs, TaggedValue numTemps, TaggedValue primitiveNumber)
    : bytes_(bytes), literals_(literals), numArgs_(numArgs), 
      numTemps_(numTemps), primitiveNumber_(primitiveNumber) {
}

} // namespace st


