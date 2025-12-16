#!/usr/bin/env python3

import os
import re
import sys


SKIP_FILES = {"mirror.hpp"}


def is_comment_or_empty(s: str) -> bool:
    t = s.strip()
    return t == "" or t.startswith("//")


def is_allowed_non_field_line(t: str) -> bool:
    # Things we allow inside class bodies that can contain ';' but are not instance fields.
    allowed_prefixes = (
        "public:",
        "private:",
        "protected:",
        "using ",
        "typedef ",
        "static ",
        "friend ",
        "enum ",
        "struct ",
        "class ",
    )
    return t.startswith(allowed_prefixes)


def check_file(path: str) -> list[str]:
    errors: list[str] = []
    with open(path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    in_class = False
    brace_depth = 0

    class_start_re = re.compile(r"^\s*(class|struct)\s+\w+")

    for i, raw in enumerate(lines, start=1):
        line = raw.rstrip("\n")
        t = line.strip()

        if is_comment_or_empty(line):
            continue

        # Start tracking once we see a class/struct definition.
        if not in_class and class_start_re.search(line):
            in_class = True
            # class line might not open brace on same line; brace tracking below handles it.

        if in_class:
            # Update brace depth.
            brace_depth += line.count("{")
            brace_depth -= line.count("}")

            # If we're in a class body (at least one '{' seen)
            if brace_depth > 0 and ";" in line:
                # Ignore method declarations/definitions.
                if "(" in line:
                    pass
                else:
                    # Ignore obvious non-field statements.
                    if is_allowed_non_field_line(t):
                        pass
                    else:
                        # Enforce slot macro for instance fields.
                        if "ST_SLOT(" not in line:
                            errors.append(f"{path}:{i}: non-slot field in mirror class. Use ST_SLOT(...): {t}")

            # Leaving the class (brace depth returns to 0 after having entered)
            if brace_depth <= 0 and "}" in line:
                in_class = False
                brace_depth = 0

    return errors


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: check_mirror_layout.py <src/classes directory>", file=sys.stderr)
        return 2

    root = sys.argv[1]
    if not os.path.isdir(root):
        print(f"error: not a directory: {root}", file=sys.stderr)
        return 2

    all_errors: list[str] = []
    for dirpath, _, filenames in os.walk(root):
        for fn in filenames:
            if not fn.endswith(".hpp"):
                continue
            if fn in SKIP_FILES:
                continue
            path = os.path.join(dirpath, fn)
            all_errors.extend(check_file(path))

    if all_errors:
        print("Mirror layout violations found:\n", file=sys.stderr)
        for e in all_errors:
            print(e, file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())


