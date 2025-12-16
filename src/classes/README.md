## `src/classes/` contract (Smalltalk-mirror types)

This directory contains **C++ classes that mirror Smalltalk classes/objects**.

### Hard invariants

- **Slot-only**: All _instance fields_ in mirror classes are **Smalltalk slots**, stored as `TaggedValue`.
- **Same meaning as Smalltalk**: If a Smalltalk instance variable is a SmallInteger/Array/etc, the C++ slot is still a `TaggedValue` (immediate or pointer).
- **No native storage in mirror types**: Mirror classes must not embed `std::vector`, `std::string`, raw C++ ints, or raw pointers as _fields_.
- **Indexed storage lives elsewhere**: Variable-sized/indexed parts (e.g. `Array` elements, `ByteArray` bytes) are not represented as C++ fields. They will be accessed via object memory later.

### How we enforce this

- **Declare slots with `ST_SLOT(name)` only** (see `mirror.hpp`).
- A build-time check (`MirrorLayoutCheck` via `ctest`) scans headers in this directory and fails if it finds non-slot member fields.

If you need temporary vector-backed containers for tests, use `src/runtime/` instead.
