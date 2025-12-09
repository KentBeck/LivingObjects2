# Bytecode Instruction Tests

This directory contains unit tests for Smalltalk VM bytecode instructions.

## Test Pattern

Each test follows this pattern:

1. **Create a CompiledMethod in memory** with bytecode and literals
2. **Create a Context** for executing the method
3. **Step forward one instruction**
4. **Check the post conditions**

## Example Test

```cpp
TEST(BytecodeInstructions, PushLiteral_Basic) {
    // 1. Create bytecode
    std::vector<uint8_t> bytecode = encodeInstruction(0, {0}); // PUSH_LITERAL index 0

    // 2. Create literals
    std::vector<TaggedValue> literals = {makeSmallInteger(42)};

    // 3. Create CompiledMethod
    auto method = createCompiledMethod(bytecode, literals);

    // 4. Create Context
    auto context = createContext(method.get(), makeSmallInteger(0));

    // 5. Check pre-conditions
    ASSERT_EQ(getStackDepth(context.get()), 0);

    // 6. Step one instruction
    bool success = stepInstruction(context.get());

    // 7. Check post conditions
    ASSERT_TRUE(success);
    ASSERT_EQ(getStackDepth(context.get()), 1);
    ASSERT_EQ(getStackTop(context.get()), literals[0]);
    ASSERT_EQ(getInstructionPointer(context.get()), 5);
}
```

## Building and Running Tests

```bash
# Configure build
mkdir build && cd build
cmake ..

# Build tests
cmake --build .

# Run tests
./bin/bytecode_instructions_test

# Or use ctest
ctest --output-on-failure
```

## Test Helpers

The `bytecode_test_helpers.hpp` provides utilities:

- `createCompiledMethod()` - Create a method with bytecode and literals
- `createContext()` - Create an execution context
- `stepInstruction()` - Execute one bytecode instruction
- `getStack()`, `getStackTop()`, `getStackDepth()` - Inspect stack state
- `getInstructionPointer()` - Get current IP
- `encodeInstruction()` - Encode bytecode with operands
- `makeSmallInteger()`, `makeNil()`, `makeTrue()`, `makeFalse()` - Create TaggedValues

## Test Coverage

Tests are organized by bytecode instruction:

- **PUSH_LITERAL (0)**: Push literal from literals array
- **PUSH_SELF (3)**: Push self (receiver) onto stack
- **POP (11)**: Pop and discard top of stack
- More instructions to be added...

Each instruction should have tests for:

- Normal operation
- Edge cases (empty stack, out of bounds, etc.)
- Error conditions
- Stack effects
- Instruction pointer advancement

## Post Conditions to Check

For each instruction, verify:

1. **Stack effect**: Correct number of values pushed/popped
2. **Stack contents**: Correct values on stack
3. **Instruction pointer**: Advanced by correct number of bytes
4. **Error handling**: Appropriate errors for invalid conditions
5. **Context state**: Other context fields unchanged (if applicable)

## Reference

See `specs/bytecode/schema/` for bytecode instruction specifications.
