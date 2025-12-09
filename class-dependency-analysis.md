# Class Dependency Analysis

## Dependency Graph

### Foundation Layer (No Dependencies)

1. **TaggedValue** - Tagged value encoding/decoding
2. **ObjectHeader** - Object header structure
3. **MemoryManager** - Heap allocation

### Base Layer (Depends on Foundation)

4. **Object** - Base object representation
   - Depends on: ObjectHeader, TaggedValue, MemoryManager

### Bootstrap Problem: Class Circular Dependencies

The challenge is that:

- **Class** needs **IdentityDictionary** (for method dictionaries)
- **IdentityDictionary** needs **Class** (it's a class object!)
- **All special object types** need **Class** (they're all classes)
- **Symbol** needs **String** (for string data)
- **String** needs **Character** (for characters)

### Solution: Two-Phase Class Implementation

#### Phase 2.1: Minimal Class Structure

5. **Class (minimal)** - Basic class structure without method dictionaries
   - Depends on: Object
   - Has: superclass pointer, instance variable count, class name (as raw string initially)
   - Does NOT have: method dictionary yet

#### Phase 2.2: Special Object Type Classes (using minimal Class)

6. **SmallInteger class** - Class object for SmallInteger

   - Depends on: Object, Class (minimal)
   - Values are immediate (tagged), but class object is heap-allocated

7. **Character class** - Class object for Character

   - Depends on: Object, Class (minimal)
   - Values are immediate (tagged), but class object is heap-allocated

8. **Array class** - Class object for Array

   - Depends on: Object, Class (minimal)
   - Array instances need Array class

9. **ByteArray class** - Class object for ByteArray

   - Depends on: Object, Class (minimal)
   - ByteArray instances need ByteArray class

10. **String class** - Class object for String

    - Depends on: Object, Class (minimal), Character class
    - String instances contain Character values

11. **Symbol class** - Class object for Symbol

    - Depends on: Object, Class (minimal), String class
    - Symbol instances contain String data

12. **IdentityDictionary class** - Class object for IdentityDictionary
    - Depends on: Object, Class (minimal), Array class
    - IdentityDictionary instances use Array for storage

#### Phase 2.3: Complete Class with Method Dictionaries

13. **Class (complete)** - Add method dictionary support
    - Depends on: IdentityDictionary class (now available)
    - Add: method dictionary (IdentityDictionary with Symbol keys)
    - Now classes can have methods!

#### Phase 2.4: CompiledMethod

14. **CompiledMethod class** - Class object for CompiledMethod
    - Depends on: Object, Class (complete), ByteArray class, Array class
    - CompiledMethod instances contain bytecode (ByteArray) and literals (Array)

## Detailed Dependency Chain

```
TaggedValue (no deps)
  └─> Object (needs TaggedValue, ObjectHeader, MemoryManager)
      └─> Class (minimal) (needs Object)
          ├─> SmallInteger class (needs Class minimal)
          ├─> Character class (needs Class minimal)
          ├─> Array class (needs Class minimal)
          ├─> ByteArray class (needs Class minimal)
          ├─> String class (needs Class minimal, Character class)
          │   └─> Symbol class (needs Class minimal, String class)
          │       └─> IdentityDictionary class (needs Class minimal, Array class)
          │           └─> Class (complete) (needs IdentityDictionary class)
          │               └─> CompiledMethod class (needs Class complete, ByteArray, Array)
```

## Correct Implementation Order

### Phase 1: Foundation

1. TaggedValue
2. ObjectHeader
3. MemoryManager

### Phase 2: Object Model (in this exact order)

#### 2.1 Base Object

4. Object

#### 2.2 Minimal Class Structure

5. Class (minimal) - without method dictionaries

#### 2.3 Special Object Type Classes

6. SmallInteger class
7. Character class
8. Array class
9. ByteArray class
10. String class (depends on Character)
11. Symbol class (depends on String)
12. IdentityDictionary class (depends on Array, Symbol)

#### 2.4 Complete Class System

13. Class (complete) - add method dictionary support

#### 2.5 CompiledMethod

14. CompiledMethod class

### Phase 3: Execution

15. Context class (depends on CompiledMethod)
16. Interpreter (depends on Context, CompiledMethod)

## Key Insights

1. **Bootstrap Problem**: Classes need IdentityDictionary, but IdentityDictionary is a class. Solution: Create minimal Class first, then special classes, then IdentityDictionary, then complete Class.

2. **Character before String**: String contains Character values, so Character class must come first.

3. **String before Symbol**: Symbol contains String data, so String class must come first.

4. **Array before IdentityDictionary**: IdentityDictionary uses Array for storage, so Array class must come first.

5. **Symbol before IdentityDictionary**: IdentityDictionary method dictionaries use Symbol keys, so Symbol class must come first.

6. **IdentityDictionary before Class (complete)**: Class method dictionaries use IdentityDictionary, so IdentityDictionary class must come before completing Class.

7. **ByteArray and Array before CompiledMethod**: CompiledMethod uses ByteArray for bytecode and Array for literals.

## Implementation Notes

- **Minimal Class**: Can be implemented with a placeholder for method dictionary (null or empty), then filled in later.
- **Class Creation**: When creating class objects, they need to be instances of Class, but Class itself is also a class (metaclass).
- **Metaclass Handling**: May need to handle Class's own class specially during bootstrap.

## Testing Order

Tests should follow the same dependency order:

1. Test TaggedValue, ObjectHeader, MemoryManager independently
2. Test Object (with minimal Class)
3. Test each special class as it's created
4. Test Class (complete) with method dictionaries
5. Test CompiledMethod
6. Test Context and Interpreter

