# C++ Virtual Machine Implementation Plan

**Based on**: Smalltalk VM Specification v1.0 from [KentBeck/LivingObjects](https://github.com/KentBeck/LivingObjects)  
**Target**: Conformant C++ implementation  
**Date**: 2025-01-05

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Core Components](#core-components)
4. [Implementation Phases](#implementation-phases)
5. [Detailed Component Specifications](#detailed-component-specifications)
6. [Testing Strategy](#testing-strategy)
7. [Build System](#build-system)
8. [Compliance Checklist](#compliance-checklist)

---

## 1. Overview

### 1.1 Goals

- Implement a fully conformant Smalltalk VM in C++
- Support all 15 bytecode instructions
- Implement all specified primitives (40+)
- Support image file format (save/load)
- Pass all test suites from the specifications
- Maintain clean, maintainable C++ code

### 1.2 Design Principles

- **Correctness First**: Follow specifications exactly
- **Modularity**: Clear separation of concerns
- **Testability**: Each component independently testable
- **Performance**: Reasonable performance without premature optimization
- **Standards**: Modern C++ (C++17 or later)

### 1.3 Key Specifications

- **Tagged Values**: 2-bit tagging for immediate values
- **Object Memory**: 64-bit headers with type/flags
- **Bytecode**: 15 opcodes with little-endian operands
- **Primitives**: 40+ primitive methods
- **Image Format**: Binary format with magic "STLK"
- **Context Model**: Method and block contexts with home chains

---

## 2. Architecture

### 2.1 High-Level Architecture

```
┌─────────────────────────────────────────────────┐
│              Application Layer                  │
│  (Image Loader, Compiler, REPL, etc.)          │
└─────────────────────────────────────────────────┘
                      │
┌─────────────────────────────────────────────────┐
│              Virtual Machine Core               │
│  ┌──────────────┐  ┌──────────────┐            │
│  │ Interpreter  │  │ Primitives   │            │
│  └──────────────┘  └──────────────┘            │
│  ┌──────────────┐  ┌──────────────┐            │
│  │ Memory Mgmt  │  │ GC           │            │
│  └──────────────┘  └──────────────┘            │
└─────────────────────────────────────────────────┘
                      │
┌─────────────────────────────────────────────────┐
│              Object Model                       │
│  (TaggedValue, Objects, Classes, Methods)      │
└─────────────────────────────────────────────────┘
```

### 2.2 Core Modules

1. **TaggedValue**: Tagged value representation
2. **ObjectModel**: Object headers, classes, methods
3. **MemoryManager**: Heap allocation and management
4. **GarbageCollector**: Mark-sweep or generational GC
5. **Interpreter**: Bytecode execution engine
6. **Primitives**: Native method implementations
7. **ImageIO**: Image file save/load
8. **SymbolTable**: Symbol interning
9. **ExceptionHandler**: Exception handling mechanism

---

## 3. Core Components

### 3.1 Component List

| Component           | Files                    | Description                    |
| ------------------- | ------------------------ | ------------------------------ |
| TaggedValue         | `tagged_value.hpp/cpp`   | Tagged value encoding/decoding |
| ObjectHeader        | `object_header.hpp/cpp`  | Object header structure        |
| Object              | `object.hpp/cpp`         | Base object representation     |
| Class               | `class.hpp/cpp`          | Class objects and hierarchy    |
| Method              | `method.hpp/cpp`         | CompiledMethod representation  |
| Context             | `context.hpp/cpp`        | Method and block contexts      |
| MemoryManager       | `memory_manager.hpp/cpp` | Heap allocation                |
| GarbageCollector    | `gc.hpp/cpp`             | Garbage collection             |
| Interpreter         | `interpreter.hpp/cpp`    | Bytecode interpreter           |
| PrimitiveDispatcher | `primitives.hpp/cpp`     | Primitive method dispatch      |
| ImageLoader         | `image_loader.hpp/cpp`   | Image file loading             |
| ImageSaver          | `image_saver.hpp/cpp`    | Image file saving              |
| SymbolTable         | `symbol_table.hpp/cpp`   | Symbol interning               |
| VM                  | `vm.hpp/cpp`             | Main VM class                  |

---

## 4. Implementation Phases

### Phase 1: Foundation (Week 1-2)

**Goal**: Core data structures and memory management

#### 1.1 TaggedValue Implementation

- [ ] Define `TaggedValue` type (uintptr_t or uint64_t)
- [ ] Implement tag encoding/decoding functions
- [ ] Implement SmallInteger encoding/decoding
- [ ] Implement special values (nil, true, false)
- [ ] Implement pointer encoding/decoding
- [ ] Unit tests for all tag operations

**Files**: `src/tagged_value.hpp`, `src/tagged_value.cpp`, `tests/tagged_value_test.cpp`

#### 1.2 Object Header

- [ ] Define `ObjectHeader` struct (64 bits)
- [ ] Implement size field (24 bits)
- [ ] Implement type field (3 bits)
- [ ] Implement flags field (5 bits)
- [ ] Implement hash field (32 bits)
- [ ] Accessor methods for all fields
- [ ] Unit tests

**Files**: `src/object_header.hpp`, `src/object_header.cpp`, `tests/object_header_test.cpp`

#### 1.3 Basic Memory Management

- [ ] Implement `MemoryManager` class
- [ ] Heap allocation (aligned to 4 bytes)
- [ ] Object allocation with header
- [ ] Basic memory tracking
- [ ] Memory alignment utilities
- [ ] Unit tests

**Files**: `src/memory_manager.hpp`, `src/memory_manager.cpp`, `tests/memory_manager_test.cpp`

### Phase 2: Object Model (Week 3-4)

**Goal**: Object representation and class system

#### 2.1 Base Object

- [ ] Implement `Object` class
- [ ] Object layout (header + class pointer + instance vars)
- [ ] Instance variable access
- [ ] Object type checking
- [ ] Unit tests

**Files**: `src/object.hpp`, `src/object.cpp`, `tests/object_test.cpp`

#### 2.2 Special Object Types

- [ ] SmallInteger class (immediate values use tagged encoding, but class object needed)
- [ ] Character class (immediate values use tagged encoding, but class object needed)
- [ ] Array implementation
- [ ] String implementation (depends on Character)
- [ ] Symbol implementation
- [ ] ByteArray implementation
- [ ] IdentityDictionary implementation (uses == for key comparison)
- [ ] Unit tests for each

**Files**: `src/small_integer.hpp/cpp`, `src/character.hpp/cpp`, `src/array.hpp/cpp`, `src/string.hpp/cpp`, `src/symbol.hpp/cpp`, `src/byte_array.hpp/cpp`, `src/identity_dictionary.hpp/cpp`

**Note**: Special object types must be implemented before Class and CompiledMethod, as they are needed for method dictionaries, class names, bytecode storage, etc.

**SmallInteger Notes**:

- SmallInteger values are immediate (encoded in tagged values, Phase 1)
- SmallInteger class object is a heap-allocated Class object needed for the class hierarchy
- Used for primitive method dispatch (primitives 1-11)

**Character Notes**:

- Character values are immediate (encoded in tagged values, Phase 1)
- Character class object is a heap-allocated Class object needed for the class hierarchy
- String operations (primitives 63-64) work with Character values
- Character encoding: Unicode code point stored in tagged value

**IdentityDictionary vs Dictionary**:

- **IdentityDictionary**: Uses identity equality (`==`) for key comparison. Used for method dictionaries where Symbol keys must match by identity.
- **Dictionary**: Uses value equality (`=`) for key comparison. Implemented later (Phase 4 or 5) as it depends on value equality semantics.

#### 2.3 Class System

- [ ] Implement `Class` class
- [ ] Class hierarchy (superclass chain)
- [ ] Instance variable definitions
- [ ] Method dictionary (uses IdentityDictionary with Symbol keys)
- [ ] Method lookup (with inheritance)
- [ ] Unit tests

**Files**: `src/class.hpp`, `src/class.cpp`, `tests/class_test.cpp`

**Note**: Class system depends on IdentityDictionary for method dictionaries (Symbol keys use identity equality) and Symbol for class names.

#### 2.4 CompiledMethod

- [ ] Implement `CompiledMethod` class
- [ ] Method layout (header + primitive + bytecode + literals)
- [ ] Bytecode access (uses ByteArray)
- [ ] Literal access (uses Array)
- [ ] Primitive number access
- [ ] Unit tests

**Files**: `src/method.hpp`, `src/method.cpp`, `tests/method_test.cpp`

**Note**: CompiledMethod depends on ByteArray for bytecode storage and Array for literals.

### Phase 3: Execution Engine (Week 5-7)

**Goal**: Bytecode interpreter and context management

#### 3.1 Context Implementation

- [ ] Implement `Context` class
- [ ] Method context layout
- [ ] Block context layout
- [ ] Stack management
- [ ] Temporary variable access
- [ ] Home context chain traversal
- [ ] Unit tests

**Files**: `src/context.hpp`, `src/context.cpp`, `tests/context_test.cpp`

#### 3.2 Bytecode Interpreter

- [ ] Implement `Interpreter` class
- [ ] Instruction fetch and decode
- [ ] Stack operations (push/pop)
- [ ] Instruction pointer management
- [ ] Context switching
- [ ] Unit tests for each bytecode

**Files**: `src/interpreter.hpp`, `src/interpreter.cpp`, `tests/interpreter_test.cpp`

#### 3.3 Bytecode Instructions

- [ ] PUSH_LITERAL (0)
- [ ] PUSH_INSTANCE_VARIABLE (1)
- [ ] PUSH_TEMPORARY_VARIABLE (2)
- [ ] PUSH_SELF (3)
- [ ] STORE_INSTANCE_VARIABLE (4)
- [ ] STORE_TEMPORARY_VARIABLE (5)
- [ ] SEND_MESSAGE (6)
- [ ] RETURN_STACK_TOP (7)
- [ ] JUMP (8)
- [ ] JUMP_IF_TRUE (9)
- [ ] JUMP_IF_FALSE (10)
- [ ] POP (11)
- [ ] DUPLICATE (12)
- [ ] CREATE_BLOCK (13)
- [ ] EXECUTE_BLOCK (14)

**Test**: Use bytecode schema JSON files to generate test cases

### Phase 4: Primitives (Week 8-10)

**Goal**: Implement all primitive methods

#### 4.1 Primitive Infrastructure

- [ ] Implement `PrimitiveDispatcher` class
- [ ] Primitive lookup table
- [ ] Primitive failure handling
- [ ] Fallback to Smalltalk code
- [ ] Unit tests

**Files**: `src/primitives.hpp`, `src/primitives.cpp`, `tests/primitives_test.cpp`

#### 4.2 Integer Primitives (1-11)

- [ ] Primitive 1: `+` (addition)
- [ ] Primitive 2: `-` (subtraction)
- [ ] Primitive 3: `<` (less than)
- [ ] Primitive 4: `>` (greater than)
- [ ] Primitive 5: `<=` (less than or equal)
- [ ] Primitive 6: `>=` (greater than or equal)
- [ ] Primitive 7: `=` (equality)
- [ ] Primitive 8: `~=` (inequality)
- [ ] Primitive 9: `*` (multiplication)
- [ ] Primitive 10: `/` (division)
- [ ] Primitive 11: `//` (modulo)

**Test**: Use primitive schema JSON files to generate test cases

#### 4.3 Array Primitives (60-62)

- [ ] Primitive 60: `at:`
- [ ] Primitive 61: `at:put:`
- [ ] Primitive 62: `size`

#### 4.4 String Primitives (63-67)

- [ ] Primitive 63: `at:`
- [ ] Primitive 64: `at:put:`
- [ ] Primitive 65: `,` (concatenation)
- [ ] Primitive 66: `size`
- [ ] Primitive 67: `asSymbol`

#### 4.5 Object Primitives (70-75, 111)

- [ ] Primitive 70: `new`
- [ ] Primitive 71: `basicNew`
- [ ] Primitive 72: `basicNew:`
- [ ] Primitive 75: `identityHash`
- [ ] Primitive 111: `class`

#### 4.6 Block Primitives (201-202)

- [ ] Primitive 201: `Block>>value`
- [ ] Primitive 202: `Block>>value:`

#### 4.8 Dictionary Primitives (700-703)

- [ ] Primitive 700: `at:`
- [ ] Primitive 701: `at:put:`
- [ ] Primitive 702: `keys`
- [ ] Primitive 703: `size`

#### 4.9 Exception Primitives (1000-1001)

- [ ] Primitive 1000: Exception handler marker
- [ ] Primitive 1001: Exception signaling

### Phase 5: Image I/O (Week 11-12)

**Goal**: Image file save and load

#### 5.1 Image Format Implementation

- [ ] Image header structure
- [ ] Class table serialization
- [ ] Global dictionary serialization
- [ ] Object graph serialization
- [ ] Reference resolution
- [ ] Unit tests

**Files**: `src/image_format.hpp`, `src/image_format.cpp`, `tests/image_format_test.cpp`

#### 5.2 Image Loader

- [ ] Read and validate header
- [ ] Load class table
- [ ] Load global dictionary
- [ ] Load object data
- [ ] Resolve object references
- [ ] Reconnect primitives
- [ ] Unit tests

**Files**: `src/image_loader.hpp`, `src/image_loader.cpp`, `tests/image_loader_test.cpp`

#### 5.3 Image Saver

- [ ] Traverse object graph
- [ ] Assign object IDs
- [ ] Topological sort
- [ ] Write header
- [ ] Write class table
- [ ] Write global dictionary
- [ ] Write object data
- [ ] Unit tests

**Files**: `src/image_saver.hpp`, `src/image_saver.cpp`, `tests/image_saver_test.cpp`

### Phase 6: Garbage Collection (Week 13-14)

**Goal**: Memory management and GC

#### 6.1 Basic Mark-Sweep GC

- [ ] Mark phase (mark all reachable objects)
- [ ] Sweep phase (free unmarked objects)
- [ ] Root set identification
- [ ] Object marking
- [ ] Unit tests

**Files**: `src/gc.hpp`, `src/gc.cpp`, `tests/gc_test.cpp`

#### 6.2 GC Integration

- [ ] GC triggers (allocation pressure)
- [ ] GC during execution
- [ ] Object forwarding
- [ ] Remembered set (for generational GC)
- [ ] Unit tests

### Phase 7: Symbol Table (Week 15)

**Goal**: Symbol interning

#### 7.1 Symbol Table

- [ ] Symbol interning
- [ ] Symbol lookup
- [ ] Symbol equality (pointer equality)
- [ ] Unit tests

**Files**: `src/symbol_table.hpp`, `src/symbol_table.cpp`, `tests/symbol_table_test.cpp`

### Phase 8: Exception Handling (Week 16)

**Goal**: Exception mechanism

#### 8.1 Exception System

- [ ] Exception classes
- [ ] Exception signaling
- [ ] Handler search
- [ ] Context unwinding
- [ ] ensure: blocks
- [ ] Unit tests

**Files**: `src/exception.hpp`, `src/exception.cpp`, `tests/exception_test.cpp`

### Phase 9: Integration and Testing (Week 17-18)

**Goal**: Full system integration and compliance testing

#### 9.1 VM Integration

- [ ] Main VM class
- [ ] VM initialization
- [ ] VM execution loop
- [ ] VM shutdown
- [ ] Integration tests

**Files**: `src/vm.hpp`, `src/vm.cpp`, `tests/vm_test.cpp`

#### 9.2 Compliance Testing

- [ ] Run all expression tests
- [ ] Run all bytecode tests
- [ ] Run all primitive tests
- [ ] Run all image format tests
- [ ] Fix any failures
- [ ] Performance testing

**Files**: `tests/compliance_test.cpp`

#### 9.3 Documentation

- [ ] API documentation
- [ ] Architecture documentation
- [ ] Implementation notes
- [ ] User guide

---

## 5. Detailed Component Specifications

### 5.1 TaggedValue

```cpp
// tagged_value.hpp
class TaggedValue {
public:
    using Value = uintptr_t;

    // Tag constants
    static constexpr Value TAG_POINTER = 0x00;
    static constexpr Value TAG_SPECIAL = 0x01;
    static constexpr Value TAG_FLOAT = 0x02;
    static constexpr Value TAG_INTEGER = 0x03;

    // Special values
    static constexpr Value NIL = 0x00000001;
    static constexpr Value TRUE = 0x00000005;
    static constexpr Value FALSE = 0x00000009;

    // Construction
    TaggedValue() = default;
    explicit TaggedValue(Value value) : value_(value) {}

    // Type checking
    bool isPointer() const;
    bool isSmallInteger() const;
    bool isSpecial() const;
    bool isNil() const;
    bool isTrue() const;
    bool isFalse() const;
    bool isBoolean() const;

    // Encoding/decoding
    static TaggedValue fromSmallInteger(int32_t n);
    int32_t toSmallInteger() const;

    static TaggedValue fromPointer(void* ptr);
    void* toPointer() const;

    // Special values
    static TaggedValue nil();
    static TaggedValue trueValue();
    static TaggedValue falseValue();

    // Access
    Value value() const { return value_; }

private:
    Value value_;
};
```

### 5.2 ObjectHeader

```cpp
// object_header.hpp
class ObjectHeader {
public:
    // Object types
    enum Type : uint8_t {
        TYPE_IMMEDIATE = 0,
        TYPE_OBJECT = 1,
        TYPE_ARRAY = 2,
        TYPE_BYTE_ARRAY = 3,
        TYPE_SYMBOL = 4,
        TYPE_CONTEXT = 5,
        TYPE_CLASS = 6,
        TYPE_METHOD = 7
    };

    // Flags
    static constexpr uint8_t FLAG_MARKED = 1 << 0;
    static constexpr uint8_t FLAG_REMEMBERED = 1 << 1;
    static constexpr uint8_t FLAG_IMMUTABLE = 1 << 2;
    static constexpr uint8_t FLAG_FORWARDED = 1 << 3;
    static constexpr uint8_t FLAG_PINNED = 1 << 4;
    static constexpr uint8_t FLAG_CONTAINS_POINTERS = 1 << 5;

    // Accessors
    uint32_t size() const;
    void setSize(uint32_t size);

    Type type() const;
    void setType(Type type);

    uint8_t flags() const;
    void setFlags(uint8_t flags);
    bool hasFlag(uint8_t flag) const;
    void setFlag(uint8_t flag);
    void clearFlag(uint8_t flag);

    uint32_t hash() const;
    void setHash(uint32_t hash);

private:
    uint64_t header_;

    // Layout: [size:24][type:3][flags:5][hash:32]
    static constexpr int SIZE_SHIFT = 0;
    static constexpr int TYPE_SHIFT = 24;
    static constexpr int FLAGS_SHIFT = 27;
    static constexpr int HASH_SHIFT = 32;
};
```

### 5.3 Interpreter

```cpp
// interpreter.hpp
class Interpreter {
public:
    Interpreter(VM* vm);

    // Execution
    TaggedValue executeMethod(CompiledMethod* method,
                             TaggedValue receiver,
                             const std::vector<TaggedValue>& args);

    TaggedValue executeBlock(BlockContext* block,
                            const std::vector<TaggedValue>& args);

    // Context management
    Context* currentContext() const { return current_context_; }
    void setCurrentContext(Context* context);

    // Stack operations
    void push(TaggedValue value);
    TaggedValue pop();
    TaggedValue top() const;

private:
    // Bytecode execution
    void executeBytecode(Context* context);
    void dispatchInstruction(uint8_t opcode, Context* context);

    // Instruction implementations
    void pushLiteral(Context* context);
    void pushInstanceVariable(Context* context);
    void pushTemporaryVariable(Context* context);
    void pushSelf(Context* context);
    void storeInstanceVariable(Context* context);
    void storeTemporaryVariable(Context* context);
    void sendMessage(Context* context);
    void returnStackTop(Context* context);
    void jump(Context* context);
    void jumpIfTrue(Context* context);
    void jumpIfFalse(Context* context);
    void pop(Context* context);
    void duplicate(Context* context);
    void createBlock(Context* context);
    void executeBlock(Context* context);

    VM* vm_;
    Context* current_context_;
};
```

### 5.4 PrimitiveDispatcher

```cpp
// primitives.hpp
class PrimitiveDispatcher {
public:
    using PrimitiveFunction = std::function<TaggedValue(
        VM* vm,
        TaggedValue receiver,
        const std::vector<TaggedValue>& args
    )>;

    PrimitiveDispatcher(VM* vm);

    // Primitive execution
    bool tryPrimitive(uint32_t primitiveNumber,
                     TaggedValue receiver,
                     const std::vector<TaggedValue>& args,
                     TaggedValue& result);

    // Primitive registration
    void registerPrimitive(uint32_t number, PrimitiveFunction func);

private:
    VM* vm_;
    std::unordered_map<uint32_t, PrimitiveFunction> primitives_;

    // Initialize all primitives
    void initializePrimitives();
};
```

---

## 6. Testing Strategy

### 6.1 Test Structure

```
tests/
├── unit/
│   ├── tagged_value_test.cpp
│   ├── object_header_test.cpp
│   ├── object_test.cpp
│   ├── class_test.cpp
│   ├── method_test.cpp
│   ├── context_test.cpp
│   ├── interpreter_test.cpp
│   ├── primitives_test.cpp
│   ├── memory_manager_test.cpp
│   ├── gc_test.cpp
│   ├── image_loader_test.cpp
│   ├── image_saver_test.cpp
│   └── symbol_table_test.cpp
├── integration/
│   ├── bytecode_test.cpp
│   ├── primitive_test.cpp
│   └── expression_test.cpp
└── compliance/
    ├── expression_tests.cpp  (from specs/tests/expression-tests.md)
    └── image_format_tests.cpp
```

### 6.2 Test Generation

- Parse JSON schema files from `specs/` directory
- Generate test cases automatically
- Use property-based testing where appropriate
- Ensure 100% coverage of all bytecodes and primitives

### 6.3 Test Execution

```bash
# Run all tests
make test

# Run specific test suite
./build/bin/tagged_value_test
./build/bin/interpreter_test
./build/bin/compliance_test
```

---

## 7. Build System

### 7.1 CMake Structure

```
CMakeLists.txt
├── src/
│   ├── CMakeLists.txt
│   └── (all source files)
├── tests/
│   ├── CMakeLists.txt
│   └── (all test files)
└── specs/
    └── (specification files for reference)
```

### 7.2 Dependencies

- **C++17** or later
- **CMake** 3.15+
- **Google Test** (for unit testing)
- **JSON library** (for parsing spec files, e.g., nlohmann/json)

### 7.3 Build Commands

```bash
# Configure
mkdir build && cd build
cmake ..

# Build
make

# Run tests
make test
# or
ctest
```

---

## 8. Compliance Checklist

### 8.1 Tagged Value Representation

- [ ] Correct tag encoding (2 bits)
- [ ] SmallInteger encoding/decoding
- [ ] Special values (nil, true, false)
- [ ] Pointer encoding
- [ ] All edge cases handled

### 8.2 Object Memory Format

- [ ] 64-bit object headers
- [ ] Correct field layout
- [ ] All object types supported
- [ ] Flags properly managed
- [ ] Alignment requirements met

### 8.3 Bytecode Instructions

- [ ] All 15 opcodes implemented
- [ ] Correct operand encoding (little-endian)
- [ ] Stack effects correct
- [ ] Error handling correct
- [ ] All test cases pass

### 8.4 Primitive Methods

- [ ] All specified primitives implemented
- [ ] Correct stack manipulation
- [ ] Type checking
- [ ] Error handling
- [ ] Fallback to Smalltalk code
- [ ] All test cases pass

### 8.5 Image File Format

- [ ] Correct header format
- [ ] Class table serialization
- [ ] Global dictionary serialization
- [ ] Object graph serialization
- [ ] Reference resolution
- [ ] Round-trip save/load works

### 8.6 Context Management

- [ ] Method contexts
- [ ] Block contexts
- [ ] Home context chains
- [ ] Variable resolution
- [ ] Non-local returns

### 8.7 Exception Handling

- [ ] Exception signaling
- [ ] Handler search
- [ ] Context unwinding
- [ ] ensure: blocks

### 8.8 Garbage Collection

- [ ] Mark phase
- [ ] Sweep phase
- [ ] Root set identification
- [ ] Object forwarding

### 8.9 Test Suite Compliance

- [ ] All expression tests pass (92 tests)
- [ ] All bytecode tests pass
- [ ] All primitive tests pass
- [ ] All image format tests pass
- [ ] Integration tests pass

---

## 9. Implementation Notes

### 9.1 Code Style

- Use modern C++ features (C++17+)
- Follow RAII principles
- Use smart pointers where appropriate
- Prefer const correctness
- Use namespaces to avoid conflicts
- Document all public APIs

### 9.2 Error Handling

- Use exceptions for error conditions
- Provide clear error messages
- Maintain VM consistency on errors
- Use RAII for resource cleanup

### 9.3 Performance Considerations

- Inline hot paths
- Cache method lookups
- Optimize stack operations
- Use move semantics where appropriate
- Profile and optimize bottlenecks

### 9.4 Memory Management

- All objects allocated through MemoryManager
- GC handles all deallocation
- No manual memory management
- Use object pools for frequently allocated objects

---

## 10. Timeline Summary

| Phase   | Duration | Key Deliverables                         |
| ------- | -------- | ---------------------------------------- |
| Phase 1 | 2 weeks  | TaggedValue, ObjectHeader, MemoryManager |
| Phase 2 | 2 weeks  | Object, Class, Method, Special types     |
| Phase 3 | 3 weeks  | Context, Interpreter, All bytecodes      |
| Phase 4 | 3 weeks  | All primitives (40+)                     |
| Phase 5 | 2 weeks  | Image save/load                          |
| Phase 6 | 2 weeks  | Garbage collection                       |
| Phase 7 | 1 week   | Symbol table                             |
| Phase 8 | 1 week   | Exception handling                       |
| Phase 9 | 2 weeks  | Integration, testing, compliance         |

**Total**: 18 weeks (~4.5 months)

---

## 11. Success Criteria

A successful implementation must:

1. ✅ Pass all expression tests (92 tests from specs/tests/expression-tests.md)
2. ✅ Pass all bytecode tests (generated from specs/bytecode/schema/)
3. ✅ Pass all primitive tests (generated from specs/primitives/schema/)
4. ✅ Pass all image format tests
5. ✅ Load images created by other compliant VMs
6. ✅ Save images that can be loaded by other compliant VMs
7. ✅ Execute all bytecode instructions correctly
8. ✅ Implement all specified primitives
9. ✅ Handle all corner cases from the specification
10. ✅ Maintain clean, maintainable code

---

## 12. References

- **VM Specification**: `smalltalk-vm-specification.md`
- **Primitives Specification**: `smalltalk-primitives-specification.md`
- **Image Format**: `specs/image-format/schema/image-format.json`
- **Bytecode Schemas**: `specs/bytecode/schema/*.json`
- **Primitive Schemas**: `specs/primitives/schema/*.json`
- **Expression Tests**: `specs/tests/expression-tests.md`

---

**End of Implementation Plan**
