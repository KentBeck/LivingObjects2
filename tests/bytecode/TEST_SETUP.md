# Bytecode Test Setup

## Structure Created

```
tests/bytecode/
├── README.md                    # Test documentation
├── bytecode_test_helpers.hpp    # Test helper declarations
├── bytecode_test_helpers.cpp    # Test helper implementations (stubs)
└── bytecode_instructions_test.cpp # Actual test cases
```

## Test Pattern

Each test follows this pattern:

1. **Create CompiledMethod in memory** with bytecode and literals
2. **Create Context** for executing the method
3. **Step forward one instruction**
4. **Check post conditions**

## Current Status

### ✅ Created

- Test infrastructure files
- CMakeLists.txt configuration
- Test helper functions (declarations)
- Example tests for:
  - PUSH_LITERAL (opcode 0)
  - PUSH_SELF (opcode 3)
  - POP (opcode 11)

### ⏳ Pending Implementation

The test helpers currently throw `std::runtime_error` because the actual VM classes don't exist yet. Once you implement:

- `TaggedValue` class
- `CompiledMethod` class
- `Context` class
- `Interpreter` class

You'll need to update `bytecode_test_helpers.cpp` to use the actual implementations.

## Next Steps

1. Implement `TaggedValue` (Phase 1.1)
2. Implement `CompiledMethod` (Phase 2.5)
3. Implement `Context` (Phase 3.1)
4. Implement `Interpreter` (Phase 3.2)
5. Update test helpers to use real implementations
6. Run tests: `cd build && cmake .. && cmake --build . && ./bin/bytecode_instructions_test`

## Building

```bash
# Install CMake (if not installed)
# macOS: brew install cmake
# Linux: apt-get install cmake

# Configure and build
mkdir build && cd build
cmake ..
cmake --build .

# Run tests
./bin/bytecode_instructions_test
# or
ctest --output-on-failure
```

## Test Examples

See `bytecode_instructions_test.cpp` for examples of:

- Basic instruction execution
- Error condition testing
- Stack effect verification
- Instruction pointer tracking
