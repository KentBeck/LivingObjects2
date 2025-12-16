#pragma once

// Mirror-class field declaration macro.
// All Smalltalk instance variables MUST be declared using ST_SLOT(name).
//
// Example:
//   class Foo {
//   public:
//     ST_SLOT(bar);
//     ST_SLOT(baz);
//   };
//
// This is enforced by tools/check_mirror_layout.py (run via ctest).

#include "../tagged_value.hpp"

#define ST_SLOT(name) TaggedValue name


